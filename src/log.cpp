void sx::swap::swaplog( const name buyer,
                      const asset amount_in,
                      const asset amount_out,
                      const asset fee)
{
    if ( !has_auth("network.sx"_n )) require_auth( get_self() );
    if ( is_account("stats.sx"_n) ) require_recipient( "stats.sx"_n );

    // post trade - not used for price logic
    const symbol_code in_symcode = amount_in.symbol.code();
    const symbol_code out_symcode = amount_out.symbol.code();
    set_reserve( in_symcode );
    set_reserve( out_symcode );
}