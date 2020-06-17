void swapSx::update_spot_prices()
{
    swapSx::tokens_table _tokens( get_self(), get_self().value );
    swapSx::spotprices_table _spotprices( get_self(), get_self().value );
    auto spotprices = _spotprices.get_or_default();

    // ignore if base symbol does not exists
    if ( !is_token_exists( SPOT_PRICE_BASE ) ) return;

    // spot prices
    spotprices.quotes = map<symbol_code, double>{};
    for ( const auto token : _tokens ) {
        const symbol_code quote = token.sym.code();
        spotprices.quotes[quote] = get_spot_price( SPOT_PRICE_BASE, quote );
    }

    // save table
    spotprices.last_modified = current_time_point();
    spotprices.base = SPOT_PRICE_BASE;
    _spotprices.set( spotprices, get_self() );
}

double swapSx::get_spot_price( const symbol_code base, const symbol_code quote )
{
    if ( !is_token_exists( base ) ) return 0;
    if ( !is_token_exists( quote ) ) return 0;

    const vector<double> uppers = get_uppers( get_self(), base, quote );
    return uppers[0] / uppers[1];
}