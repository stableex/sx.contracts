double stable::calculate_out( const asset quantity, const symbol_code out_symcode )
{
    // settings
    stable::settings _settings( get_self(), get_self().value );
    int64_t amplifier = _settings.get().amplifier;

    // input
    const double in_amount = asset_to_double( quantity );

    // balances
    const double base = asset_to_double( get_balance( quantity.symbol.code() ) - quantity );
    const double quote = asset_to_double( get_balance( out_symcode ) );

    // depth
    const double base_depth = asset_to_double( get_depth( quantity.symbol.code() ) );
    const double quote_depth = asset_to_double( get_depth( out_symcode ) );

    // ratio
    const double base_ratio = base / base_depth;
    const double quote_ratio = quote / quote_depth;

    // upper
    const double base_upper = ( amplifier * base_depth - base_depth + (base_depth * base_ratio));
    const double quote_upper = ( amplifier * quote_depth - quote_depth + (quote_depth * quote_ratio));

    // Bancor V1 Formula
    return in_amount / ( base_upper + in_amount ) * quote_upper;
}

asset stable::calculate_fee( const asset quantity )
{
    // settings
    stable::settings _settings( get_self(), get_self().value );
    const int64_t fee = _settings.get().fee;

    // fee colleceted from incoming transfer (in basis points 1/100 of 1% )
    asset calculated_fee = quantity * fee / 10000;

    // set minimum fee to smallest decimal of asset
    if ( fee > 0 && calculated_fee.amount == 0 ) calculated_fee.amount = 1;
    check( calculated_fee < quantity, "fee exceeds quantity");
    return calculated_fee;
}
