
// for maintenance purposes
// @admin
[[eosio::action]]
void sx::recalibrate()
{
    require_auth( get_self() );

    map<symbol_code, asset> depths;

    for ( const auto account : _accounts ) {
        for ( const auto [ symcode, balance ] : account.balances ) {

            if (depths[ symcode ].symbol.code() == symcode ) depths[ symcode ] += balance;
            else depths[ symcode ] = balance;
        }
    }

    for ( const pair<symbol_code, asset> row : depths) {
        const symbol_code symcode = row.first;
        const asset depth = row.second;

        auto pool = _pools.find( symcode.raw() );
        if ( pool == _pools.end()) continue;

        _pools.modify( pool, same_payer, [&]( auto & row ) {
            row.depth = depth;
        });
        update_pool_ratio( symcode );
    }
}
