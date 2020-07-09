void swapSx::log( const name buyer,
                  const asset quantity,
                  const asset rate,
                  const asset fee,
                  const double trade_price,
                  const double spot_price,
                  const double value )
{
    require_auth( get_self() );
}