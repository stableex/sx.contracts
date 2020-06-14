vector<double> swapSx::get_uppers( const name contract, const symbol_code base_symcode, const symbol_code quote_symcode )
{
    // settings
    swapSx::settings _settings( contract, contract.value );
    swapSx::tokens_table _tokens( contract, contract.value );
    const tokens_row base = _tokens.get( base_symcode.raw(), "base symbol code does not exists" );
    const tokens_row quote = _tokens.get( quote_symcode.raw(), "quote symbol code does not exists" );
    const int64_t amplifier = _settings.get().amplifier;

    // depth
    const double base_depth = asset_to_double( base.depth );
    const double quote_depth = asset_to_double( quote.depth );

    // ratio
    const double base_ratio = static_cast<double>(base.balance.amount) / base.depth.amount;
    const double quote_ratio = static_cast<double>(quote.balance.amount) / quote.depth.amount;

    // upper
    const double base_upper = ( amplifier * base_depth - base_depth + (base_depth * base_ratio));
    const double quote_upper = ( amplifier * quote_depth - quote_depth + (quote_depth * quote_ratio));

    return vector<double>{ base_upper, quote_upper };
}
