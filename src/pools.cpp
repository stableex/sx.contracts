// @admin
[[eosio::action]]
void sx::create( const symbol_code symcode, const name contract, const symbol_code type )
{
    require_auth( get_self() );

    auto pools_itr = _pools.find( symcode.raw() );
    check( pools_itr == _pools.end(), "[symcode] pool is already created" );

    // check existing token contract
    const asset supply = token::get_supply( contract, symcode );
    check( supply.amount > 0, "supply amount must be above 0" );

    // empty asset
    const asset zero_asset = asset{ 0, supply.symbol };

    _pools.emplace( get_self(), [&]( auto & row ) {
        row.id = extended_symbol{ supply.symbol, contract };
        row.balance = zero_asset;
        row.depth = zero_asset;
        row.ratio = 0;
        row.proceeds = zero_asset;
        row.amplifier = 1;
        row.type = type;
        if ( type == USD ) row.pegged = asset{1'0000, symbol{"USD", 4}};
        else row.pegged = asset{0, symbol{"USD", 4}};
    });

    // open token balance for contract
    token::open_action open( contract, { get_self(), "active"_n });
    open.send( get_self(), supply.symbol, get_self() );
}

// @admin
[[eosio::action]]
void sx::addconnector( const symbol_code symcode, const symbol_code connector )
{
    require_auth( get_self() );

    auto& pool = _pools.get( symcode.raw(), "[symcode] pool does not exist" );
    _pools.get( connector.raw(), "[connector] pool does not exist" );

    // add symcode to connector
    _pools.modify( pool, get_self(), [&]( auto & row ) {
        check( row.connectors.find( connector )->raw() != connector.raw(), "nothing was modified");
        row.connectors.insert( connector );
    });
}

// @admin
[[eosio::action]]
void sx::delconnector( const symbol_code symcode, const symbol_code connector )
{
    require_auth( get_self() );

    auto& pool = _pools.get( symcode.raw(), "[symcode] pool does not exist");

    _pools.modify( pool, get_self(), [&]( auto & row ) {
        check( row.connectors.find( connector )->raw() == connector.raw(), "[connector] does not exists");
        row.connectors.erase( connector );
    });
}

// @admin
[[eosio::action]]
void sx::setmetadata( const symbol_code symcode, const map<name, string> metadata_json )
{
    require_auth( get_self() );

    auto pools_itr = _pools.find( symcode.raw() );
    check( pools_itr != _pools.end(), "[symcode] pool does not exist" );

    _pools.modify( pools_itr, get_self(), [&]( auto & row ) {
        row.metadata_json = metadata_json;
    });
}

// @admin
[[eosio::action]]
void sx::enable( const symbol_code symcode, const bool enabled )
{
    require_auth( get_self() );

    auto pools_itr = _pools.find( symcode.raw() );
    check( pools_itr != _pools.end(), "[symcode] pool does not exist" );

    _pools.modify( pools_itr, get_self(), [&]( auto & row ) {
        check( row.enabled != enabled, "[enabled] was not modified");
        row.enabled = enabled;
    });
}

// @admin
[[eosio::action]]
void sx::delpool( const symbol_code symcode )
{
    require_auth( get_self() );

    // cannot delete pool with ratio lower than 100%
    update_pool_ratios();
    int64_t ratio = _pools.get( symcode.raw(), "[delpool::symcode] pool does not exists" ).ratio;
    check( ratio == 0 || ratio >= 10000, "pool must have a ratio of 0% or above 100%" );

    // claim pool proceeds
    claim_pool( symcode );

    // release remaining proceeds
    for ( auto account : _proceeds ) {
        const name owner = account.owner;
        const asset balance = account.balances.at( symcode );

        if ( balance.symbol.code() == symcode ) {
            // system contract, release proceeds back to self
            if ( owner == get_self() ) {
                sub_proceeds_owner( get_self(), balance );
            }
            else {
                // move proceeds to balance
                sub_proceeds_owner( owner, balance );
                add_balance( owner, balance );
                add_depth( balance );
            }
        }
    }

    // transfer out any remaining balance
    for ( auto account : _accounts ) {
        const name owner = account.owner;
        const asset balance = account.balances.at( symcode );

        if ( balance.symbol.code() == symcode ) {
            // empty out account back to owner
            transfer_out( owner, balance, "delpool" );
        }
    }

    // delete connectors
    for ( auto pool : _pools ) {
        if ( pool.id.get_symbol().code() == symcode ) continue;
        if ( pool.connectors.find( symcode )->raw() == symcode.raw() ) {
            delconnector( pool.id.get_symbol().code(), symcode );
        }
    }

    // delete pool
    auto pool = _pools.find( symcode.raw() );
    check( pool != _pools.end(), "[symcode] pool does not exist");
    check( pool->depth.amount >= 0, "depth must be above zero");
    check( pool->proceeds.amount >= 0, "proceeds must be above zero");
    _pools.erase( pool );
}

// @admin
[[eosio::action]]
void sx::setamplifier( const symbol_code symcode, const int64_t amplifier )
{
    require_auth( get_self() );

    auto& pool = _pools.get( symcode.raw(), "[symcode] pool does not exist" );
    check( amplifier >= 1, "[amplifier] must be at least 1x");
    check( amplifier <= 125, "[amplifier] cannot be above 125x");

    _pools.modify( pool, get_self(), [&]( auto & row ) {
        check( row.amplifier != amplifier, "nothing was modified");
        row.amplifier = amplifier;
    });
}

void sx::update_pool_ratios()
{
    for (const auto pool : _pools ) {
        update_pool_ratio( pool.id.get_symbol().code() );
    }
}

void sx::update_pool_ratio( const symbol_code symcode )
{
    auto& pool = _pools.get( symcode.raw(), "[symcode] pool does not exist" );

    _pools.modify( pool, same_payer, [&]( auto & row ) {
        token::accounts _accounts( pool.id.get_contract(), get_self().value );

        const auto account = _accounts.find( symcode.raw() );

        // update balance
        if ( account != _accounts.end() ) row.balance = account->balance - pool.proceeds;
        else row.balance = asset{ 0, pool.id.get_symbol() };

        // update ratio
        if ( row.depth.amount > 0 && row.balance.amount > 0 ) row.ratio = row.balance.amount * 10000 / row.depth.amount;
        else row.ratio = 0;
    });
}

void sx::add_depth( const asset value )
{
    auto pools_itr = _pools.find( value.symbol.code().raw() );
    check( pools_itr != _pools.end(), "[symcode] pool does not exist" );

    _pools.modify( pools_itr, same_payer, [&]( auto & row ) {
        row.depth += value;
    });
}

void sx::sub_depth( const asset value )
{
    auto pools_itr = _pools.find( value.symbol.code().raw() );
    check( pools_itr != _pools.end(), "[symcode] pool does not exist" );

    _pools.modify( pools_itr, same_payer, [&]( auto & row ) {
        row.depth -= value;
        check(row.depth.amount >= 0, "depth overdrawn balance");
    });
}

void sx::add_proceeds( const asset value )
{
    auto pools_itr = _pools.find( value.symbol.code().raw() );
    check( pools_itr != _pools.end(), "[symcode] pool does not exist" );

    _pools.modify( pools_itr, same_payer, [&]( auto & row ) {
        row.proceeds += value;
    });
}

void sx::sub_proceeds( const asset value )
{
    auto pools_itr = _pools.find( value.symbol.code().raw() );
    check( pools_itr != _pools.end(), "[symcode] pool does not exist" );

    _pools.modify( pools_itr, same_payer, [&]( auto & row ) {
        row.proceeds -= value;
        check(row.proceeds.amount >= 0, "proceeds overdrawn balance");
    });
}

void sx::check_pool_contract( const symbol_code symcode, const name contract )
{
    auto pools = _pools.get( symcode.raw(), "[symcode] pool does not exist" );
    check( pools.id.get_contract() == contract, "invalid contract");
}

bool sx::pool_exists( const symbol_code symcode )
{
    auto pools = _pools.find( symcode.raw() );
    return pools != _pools.end();
}
