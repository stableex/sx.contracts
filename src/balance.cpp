void swapSx::balance()
{
    require_auth( get_self() );

    swapSx::tokens_table _tokens( get_self(), get_self().value );

    const map<symbol_code, double> spot_prices = get_spot_prices( SPOT_PRICE_BASE );
    const double min_value = get_min_value( spot_prices );
    bool is_balanced = false;

    print("min_value: " + to_string( min_value ) + "\n");

    // remove excess from pool using min value
    for ( const auto token : _tokens ) {
        const symbol_code quote = token.sym.code();
        const double spot_price = spot_prices.at( quote );
        const asset balance = token::get_balance( token.contract, get_self(), quote );

        // calculate excess value to remove from pool
        const double value = asset_to_double( balance ) * spot_price;
        const asset excess = double_to_asset( (value - min_value) / spot_price, token.sym );

        print(quote.to_string() + " excess: " + excess.to_string() + " value: " + to_string( value ) + "\n");

        if ( excess.amount > 1 ) {
            self_transfer( "sx"_n, excess, "excess" );
            set_balance( balance - excess );
            is_balanced = true;
        }
        set_depth( balance - excess );
    }
    check( is_balanced, "nothing to balance");
    update_spot_prices();
}

double swapSx::get_min_value( const map<symbol_code, double> spot_prices )
{
    swapSx::tokens_table _tokens( get_self(), get_self().value );
    double min_value = -1;

    // fetch min value from all tokens
    for ( const auto token : _tokens ) {
        const symbol_code quote = token.sym.code();
        const double spot_price = spot_prices.at( quote );
        const asset balance = token::get_balance( token.contract, get_self(), quote );
        const double value = asset_to_double( token.balance ) * spot_price;
        if ( min_value == -1 || value < min_value) min_value = value;

        print(quote.to_string() + " spot_price: " + to_string( spot_price ) + " value: " + to_string( value ) + "\n");
    }
    check( min_value != -1, "invalid min value");
    return min_value;
}