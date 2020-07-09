void swapSx::update_spot_prices()
{
    swapSx::tokens_table _tokens( get_self(), get_self().value );
    swapSx::spotprices_table _spotprices( get_self(), get_self().value );
    swapSx::settings _settings( get_self(), get_self().value );
    const symbol_code base = _settings.get().base;
    auto spotprices = _spotprices.get_or_default();

    // ignore if base symbol does not exists
    if ( !is_token_exists( base ) ) return;

    // spot prices
    spotprices.quotes = map<symbol_code, double>{};
    for ( const auto token : _tokens ) {
        const symbol_code quote = token.sym.code();
        spotprices.quotes[quote] = get_spot_price( base, quote );
    }

    // save table
    spotprices.last_modified = current_time_point();
    spotprices.base = base;
    _spotprices.set( spotprices, get_self() );
}

double swapSx::get_spot_price( const symbol_code base, const symbol_code quote )
{
    if ( !is_token_exists( base ) ) return 0;
    if ( !is_token_exists( quote ) ) return 0;

    const vector<double> uppers = get_uppers( get_self(), base, quote );
    return uppers[0] / uppers[1];
}

map<symbol_code, double> swapSx::get_spot_prices( const symbol_code base )
{
    swapSx::tokens_table _tokens( get_self(), get_self().value );
    map<symbol_code, double> spot_prices;

    for ( const auto token : _tokens ) {
        const symbol_code quote = token.sym.code();
        spot_prices[ quote ] = get_spot_price( base, quote );
    }
    return spot_prices;
}