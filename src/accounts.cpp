// @user
[[eosio::action]]
void sx::unstake( const name owner, const optional<asset> quantity )
{
    require_auth( owner );
    claim_pools();

    symbol_code symcode = quantity->symbol.code();

    // unstake all if quantity is `null`
    if ( symcode.raw() == 0 ) return unstake_all( owner );

    _accounts.get( owner.value, "[owner] account does not contain any balances" );
    const asset out_quantity = asset{ quantity->amount, quantity->symbol };

    check_available_balance( out_quantity );
    transfer_out( owner, out_quantity, "unstake");
    check_min_staked( owner, symcode );
    send_receipt( owner, "unstake"_n, list<asset>{ out_quantity });
}

void sx::transfer_out( const name to, const asset quantity, const string memo )
{
    // reduce account balance & depth
    sub_balance( to, quantity );
    sub_depth( quantity );

    // send static transfer action
    token::transfer_action transfer( get_contract( quantity.symbol.code() ), { get_self(), "active"_n });
    transfer.send( get_self(), to, quantity, memo );
}

void sx::unstake_all( const name owner )
{
    require_auth( owner );

    // claim any remaining proceeds
    list<asset> claimed = claim_staked( owner );
    if ( !claimed.empty() ) send_receipt( owner, "claim"_n, claimed );

    list<asset> assets;
    auto account = _accounts.get( owner.value, "nothing to unstake" );
    for ( const auto [ symcode, balance ] : account.balances ) {
        transfer_out( owner, balance, "unstake");
        assets.push_back( balance );
    }
    check( !assets.empty(), "nothing to unstake" );
    send_receipt( owner, "unstake"_n, assets);
}

void sx::check_available_balance( const asset quantity )
{
    const symbol_code symcode = quantity.symbol.code();
    update_pool_ratio( symcode );
    const asset balance = _pools.get( symcode.raw(), "[symcode] pool does not exist").balance;
    check( balance >= quantity, "pool does not have sufficient balance" );
}

void sx::check_min_staked( const name owner, const symbol_code symcode )
{
    const asset min_staked = _settings.get().min_staked;
    const auto account = _accounts.find( owner.value );
    if ( account == _accounts.end() ) return;

    const asset balance = account->balances.at( symcode );
    if ( balance.symbol.code() != symcode ) return;
    if ( min_staked.amount <= 0 ) return;
    if ( balance.amount <= 0 ) return;

    const double pegged = asset_to_double( get_pegged( symcode ) );
    check( asset_to_double(balance) * pegged >= asset_to_double(min_staked), "owner must have zero or minimum staked of " + min_staked.to_string());
}

void sx::sub_balance( const name owner, const asset value )
{
    auto from = _accounts.find( owner.value );
    check( from != _accounts.end(), "no balance object found" );
    const symbol_code symcode = value.symbol.code();

    _accounts.modify( from, same_payer, [&]( auto& row ) {
        check( row.balances[ symcode ].symbol.code().raw() != 0, "no balance object found" );
        row.balances[ symcode ] -= value;
        check( row.balances[ symcode ].amount >= 0, "overdrawn balance" );
        if ( row.balances[ symcode ].amount <= 0) row.balances.erase( symcode );
    });
}

void sx::add_balance( const name owner, const asset value )
{
    const name ram_payer = get_self();

    auto to = _accounts.find( owner.value );
    const symbol_code symcode = value.symbol.code();

    if ( to == _accounts.end() ) {
        _accounts.emplace( ram_payer, [&]( auto& row ){
            row.owner = owner;
            row.balances[symcode] = value;
        });
    } else {
        _accounts.modify( to, same_payer, [&]( auto& row ) {
            if (row.balances[ symcode ].symbol.code() == symcode ) {
                row.balances[ symcode ] += value;
            } else {
                row.balances[ symcode ] = value;
            }
        });
    }
}

void sx::clear_empty_balances( const name owner )
{
    const auto account = _accounts.find( owner.value );
    if ( account != _accounts.end() && account->balances.size() == 0 ) {
        _accounts.erase( account );
    }

    const auto proceeds = _proceeds.find( owner.value );
    if ( proceeds != _proceeds.end() && proceeds->balances.size() == 0 ) {
        _proceeds.erase( proceeds );
    }
}
