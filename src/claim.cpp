// @user
[[eosio::action]]
void sx::claim( const name owner, const bool staked )
{
    require_auth_or_self( owner );
    claim_pools();

    list<asset> assets;
    if ( staked ) assets = claim_staked( owner );
    else assets = claim_transfer_out( owner );

    // prevent claiming nothing
    check( !assets.empty(), "nothing to claim" );

    // receipts
    send_receipt( owner, "claim"_n, assets );
    if ( staked ) send_receipt( owner, "stake"_n, assets );
}

void sx::claim_pools()
{
    // TO-DO (OPTIONAL) - claim only applies to owner's staked symbols
    for ( const auto pool : _pools ) {
        if ( !pool.enabled ) continue;
        if ( pool.proceeds.amount <= 0 ) continue;
        if ( pool.depth.amount <= 0 ) continue;
        claim_pool( pool.id.get_symbol().code() );
    }
}

list<asset> sx::claim_staked( const name owner )
{
    // withdraw any available balances from owner
    list<asset> proceeds;

    const auto account = _proceeds.find( owner.value );
    if ( account == _proceeds.end() ) return proceeds;

    // collect all `proceeds` from account
    for ( const auto [ symcode, balance ] : account->balances ) {
        if ( balance.amount > 0 ) {
            proceeds.push_back( balance );
        }
    }

    // substract proceeds from account and move to balance
    for ( const asset proceed : proceeds ) {
        sub_proceeds_owner( owner, proceed );
        add_balance( owner, proceed );
        add_depth( proceed );
    }
    return proceeds;
}


list<asset> sx::claim_transfer_out( const name owner )
{
    // withdraw any available balances from owner
    list<asset> assets;

    const auto account = _proceeds.get( owner.value, "nothing to claim" );
    for ( const auto [ symcode, balance ] : account.balances ) {
        if ( balance.amount > 0 ) {
            sub_proceeds_owner( owner, balance );
            add_balance( owner, balance );
            add_depth( balance );
            transfer_out( owner, balance, "claim" );
            assets.push_back( balance );
        }
    }
    return assets;
}

void sx::claim_pool( const symbol_code symcode )
{
    const auto pool = _pools.get( symcode.raw(), "[symcode] pool does not exist");
    const symbol sym = pool.id.get_symbol();
    const double pegged = asset_to_double( pool.pegged );
    const double min_staked = asset_to_double( _settings.get().min_staked );
    const double proceeds = asset_to_double( pool.proceeds );

    // prevent claiming proceeds less than 0.1 USD
    if ( proceeds * pegged < 0.1 ) return;

    for ( const auto account : _accounts ) {
        const symbol_code staked_symcode = account.balances.find( symcode )->first;
        const asset staked = account.balances.find( symcode )->second;

        // skip owner
        if ( staked_symcode != symcode ) continue;
        if ( staked.amount <= 0 ) continue;

        // formula to calculate claimable amount
        const double ratio = static_cast<double>( staked.amount ) / pool.depth.amount;
        const asset quantity = double_to_asset( proceeds * ratio, sym );

        // distribute proceeds
        if (quantity.amount > 0 ) {
            add_proceeds_owner( account.owner, quantity );
            sub_proceeds( quantity );
        }
    }
}
