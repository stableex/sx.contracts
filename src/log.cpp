void swapSx::log( const name buyer,
                  const asset quantity,
                  const asset rate,
                  const asset fee,
                  const double trade_price,
                  const double spot_price,
                  const double value )
{
    require_auth( get_self() );

    // post trade - not used for price logic
    set_reserve( rate.symbol.code() );
    set_reserve( quantity.symbol.code() );
    update_volume( vector<asset>{ quantity, rate }, fee );
    update_spot_prices();
}