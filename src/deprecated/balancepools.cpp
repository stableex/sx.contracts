// @admin
[[eosio::action]]
void sx::balancepools( )
{
    require_auth( get_self() );

    update_pool_ratios();
    list<asset> assets;
    for ( const auto pool : _pools ) {
        const symbol_code symcode = pool.id.get_symbol().code();

        // check if balance & ratio is above 100%
        check( pool.ratio >= 10000, symcode.to_string() + " pool ratio must be above 100%" );
        const asset excess = pool.balance - pool.depth - pool.proceeds;
        check( excess.amount > 0, symcode.to_string() + " balance must exceed pool depth & proceeds" );

        // release excess to contract
        add_balance( get_self(), excess );
        add_depth( excess );
        assets.push_back( excess );
    }
    send_receipt( get_self(), "balancepool"_n, assets );
}