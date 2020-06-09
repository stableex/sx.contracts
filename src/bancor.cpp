double swapSx::get_bancor_output( const double base_reserve, const double quote_reserve, const double quantity )
{
    const double out = (quantity * quote_reserve) / (base_reserve + quantity);
    if ( out < 0 ) return 0;
    return out;
}

double swapSx::get_bancor_input( const double quote_reserve, const double base_reserve, const double out )
{
    const double inp = (base_reserve * out) / (quote_reserve - out);
    if ( inp < 0 ) return 0;
    return inp;
}
