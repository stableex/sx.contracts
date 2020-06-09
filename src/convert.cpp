vector<double> swapSx::get_uppers( const name contract, const symbol_code base_symcode, const symbol_code quote_symcode )
{
    // settings
    swapSx::settings _settings( contract, contract.value );
    swapSx::tokens _tokens( contract, contract.value );
    const tokens_row base = _tokens.get( base_symcode.raw(), "base symbol code does not exists" );
    const tokens_row quote = _tokens.get( quote_symcode.raw(), "quote symbol code does not exists" );
    const int64_t amplifier = _settings.get().amplifier;

    // depth
    const double base_depth = swapSx::asset_to_double( base.depth );
    const double quote_depth = asset_to_double( quote.depth );

    // ratio
    const double base_ratio = static_cast<double>(base.balance.amount) / base.depth.amount;
    const double quote_ratio = static_cast<double>(quote.balance.amount) / quote.depth.amount;

    // upper
    const double base_upper = ( amplifier * base_depth - base_depth + (base_depth * base_ratio));
    const double quote_upper = ( amplifier * quote_depth - quote_depth + (quote_depth * quote_ratio));

    return vector<double>{ base_upper, quote_upper };
}

asset swapSx::get_price( const name contract, const asset quantity, const symbol_code symcode )
{
    // quantity input
    const double in_amount = asset_to_double( quantity );

    // upper limits
    const vector<double> uppers = get_uppers( contract, quantity.symbol.code(), symcode );
    const double base_upper = uppers[0];
    const double quote_upper = uppers[1];

    // Bancor V1 Formula
    const double out = get_bancor_output( base_upper, quote_upper, in_amount );

    return double_to_asset( out, get_symbol( contract, symcode ));
}

asset swapSx::get_fee( const name contract, const asset quantity )
{
    // settings
    swapSx::settings _settings( contract, contract.value );
    const int64_t fee = _settings.get().fee;

    // fee colleceted from incoming transfer (in basis points 1/100 of 1% )
    asset calculated_fee = quantity * fee / 10000;

    // set minimum fee to smallest decimal of asset
    if ( fee > 0 && calculated_fee.amount == 0 ) calculated_fee.amount = 1;
    check( calculated_fee < quantity, "fee exceeds quantity");
    return calculated_fee;
}
