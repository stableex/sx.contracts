bool swapSx::is_token_exists( const symbol_code symcode )
{
    swapSx::tokens_table _tokens( get_self(), get_self().value );
    return ( _tokens.find( symcode.raw() ) != _tokens.end() );
}

double swapSx::get_ratio( const symbol_code symcode )
{
    // calculate ratio between depth & balance
    swapSx::tokens_table _tokens( get_self(), get_self().value );
    auto token = _tokens.get( symcode.raw(), "[symcode] token does not exist");
    const asset balance = swapSx::get_balance( symcode );
    return static_cast<double>(balance.amount) / token.depth.amount;
}

void swapSx::check_max_ratio( const symbol_code symcode )
{
    const asset balance = get_balance( symcode );
    const asset depth = get_depth( symcode );

    check( static_cast<double>(balance.amount) / depth.amount <= 5, symcode.to_string() + " balance/depth ratio cannot exceed 500%" );
}

void swapSx::check_min_ratio( const asset out )
{
    const asset balance = get_balance( out.symbol.code() );
    const asset depth = get_depth( out.symbol.code() );
    const asset remaining = balance - out;

    check( static_cast<double>(remaining.amount) / depth.amount >= 0.2, out.symbol.code().to_string() + " balance/depth ratio must be above 20%" );
}

void swapSx::check_is_active( const symbol_code symcode, const name contract )
{
    swapSx::tokens_table _tokens( get_self(), get_self().value );
    auto token = _tokens.find( symcode.raw() );
    check( token != _tokens.end(), symcode.to_string() + " token not available");

    if ( contract.value ) check( contract == token->contract, symcode.to_string() + " contract mismatch");
}