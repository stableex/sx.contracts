
// @user
[[eosio::action]]
asset sx::autoconvert( const name owner, const asset quantity, const set<symbol_code> symcodes, const optional<asset> profit )
{
    require_auth( owner );

    const bool symcodes_is_empty = symcodes.empty();

    check( symcodes_is_empty || symcodes.size() >= 2, "[symcodes] must be empty or have 2 or greater symbol codes" );

    // optional profit
    const bool is_profit = profit->symbol.code().raw() ? true : false;
    double profit_double;

    if ( is_profit ) {
        check( profit->symbol.code() == USD, "[profit] must be in USD (ex: 0.01 USD)");
        profit_double = asset_to_double( asset{ profit->amount, profit->symbol } );
    }

    // define incoming quantity asset
    const map<symbol_code, asset> balances = _accounts.get( owner.value, "[owner] does not contain any balances" ).balances;
    check( !balances.empty(), "[owner] does not contain any balances");

    // calculate highest ratio from incoming => outgoing asset
    symbol_code in_symcode = quantity.symbol.code();
    asset in_quantity;
    symbol_code out_symcode;
    auto in_pool = _pools.find( in_symcode.raw() );

    // pegged valuation
    double in_pegged = 1.0;
    if ( in_symcode != USD ) {
        in_pegged = asset_to_double( get_pegged( in_symcode ) );
        check( in_pool->enabled, in_symcode.to_string() + " pool is not enabled");
        if ( !symcodes_is_empty ) check( symcodes.count( in_symcode ), "[symcodes] must contain quantity symbol code" );
    }

    // max values to find the final result
    const double in_double = asset_to_double( quantity );
    const double in_value = in_double * in_pegged;
    double highest_out_delta = in_value * -1;
    double highest_out_value;

    for ( const auto [ symcode, balance ] : balances ) {
        // exclude same incoming symbol
        if ( in_symcode != USD && symcode != in_symcode ) continue;

        // only support user defined symcodes
        if ( !symcodes_is_empty && !symcodes.count( symcode ) ) continue;

        // skip any disabled pools
        const auto balance_pool = _pools.get( symcode.raw() );
        if ( !balance_pool.enabled ) continue;

        // exclude balances that don't meet minimum input quantity threshold
        const double balance_pegged = asset_to_double( get_pegged( symcode ) );
        const double balance_value = asset_to_double( balance ) * balance_pegged;
        const asset in_balance = double_to_asset( in_value / balance_pegged, balance.symbol );
        if ( balance_value < in_value ) continue;

        for ( const auto out_pool : _pools ) {
            const symbol_code pool_symcode = out_pool.id.get_symbol().code();
            if ( symcode == pool_symcode ) continue;
            if ( balance.amount <= 0 ) continue;
            if ( !out_pool.enabled ) continue;
            if ( !symcodes_is_empty && !symcodes.count( pool_symcode ) ) continue;

            // pool must have connector of balance
            if ( !out_pool.connectors.count( symcode ) ) continue;

            // estimated out (pre-fees)
            const double out_double = asset_to_double( calculate_out( in_balance, pool_symcode ) );
            const double out_pegged = asset_to_double( get_pegged( pool_symcode ) );
            const double out_value = out_double * out_pegged;
            const double out_delta = out_value - in_value;

            // skip if below 0% in delta
            if ( out_delta > highest_out_delta ) {
                highest_out_delta = out_delta;
                highest_out_value = out_value;
                out_symcode = pool_symcode;
                in_quantity = in_balance;
            }
        }
    }

    // check conversions are positive
    check( out_symcode.raw() != 0 && in_symcode.raw() != 0, "`autoconvert` cannot detect any trades");
    const asset out = convert( owner, in_quantity, out_symcode );
    if ( is_profit ) check( highest_out_value - in_value >= profit_double, "`autoconvert` cannot detect a trade which meets the minimum profit threshold");

    return out;
}
