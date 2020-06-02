void stable::add_depth( const asset value )
{
    stable::tokens _tokens( get_self(), get_self().value );
    auto itr = _tokens.find( value.symbol.code().raw() );
    check( itr != _tokens.end(), "[symcode] token does not exist" );

    _tokens.modify( itr, same_payer, [&]( auto & row ) {
        row.depth += value;
    });
}

void stable::sub_depth( const asset value )
{
    stable::tokens _tokens( get_self(), get_self().value );
    auto itr = _tokens.find( value.symbol.code().raw() );
    check( itr != _tokens.end(), "[symcode] token does not exist" );

    _tokens.modify( itr, same_payer, [&]( auto & row ) {
        row.depth -= value;
        check(row.depth.amount >= 0, "depth overdrawn balance");
    });
}

name stable::get_contract( const symbol_code symcode )
{
    return get_extended_symbol( symcode ).get_contract();
}

symbol stable::get_symbol( const symbol_code symcode )
{
    return get_extended_symbol( symcode ).get_symbol();
}

asset stable::get_balance( const symbol_code symcode )
{
    return token::get_balance( get_contract( symcode ), get_self(), symcode );
}

extended_symbol stable::get_extended_symbol( const symbol_code symcode )
{
    stable::tokens _tokens( get_self(), get_self().value );
    auto token = _tokens.find( symcode.raw() );
    check( token != _tokens.end(), symcode.to_string() + " cannot find token");
    return extended_symbol{ token->sym, token->contract };
}

double stable::get_ratio( const symbol_code symcode )
{
    // calculate ratio between depth & balance
    stable::tokens _tokens( get_self(), get_self().value );
    auto token = _tokens.get( symcode.raw(), "[symcode] token does not exist");
    const asset balance = stable::get_balance( symcode );
    return static_cast<double>(balance.amount) / token.depth.amount;
}

void stable::check_max_ratio( const asset quantity )
{
    // validate input
    stable::tokens _tokens( get_self(), get_self().value );
    auto token = _tokens.get( quantity.symbol.code().raw(), "[symcode] token does not exist");
    const asset remaining = token.balance + quantity;

    check( static_cast<double>(remaining.amount) / token.depth.amount <= 5, quantity.symbol.code().to_string() + " ratio must be lower than 500%" );
}

void stable::check_min_ratio( const asset out )
{
    // validate input
    stable::tokens _tokens( get_self(), get_self().value );
    auto token = _tokens.get( out.symbol.code().raw(), "[symcode] token does not exist");
    const asset remaining = token.balance - out;

    check( static_cast<double>(remaining.amount) / token.depth.amount >= 0.2, out.symbol.code().to_string() + " ratio must be above 20%" );
}

void stable::check_is_active( const symbol_code symcode, const name contract )
{
    stable::tokens _tokens( get_self(), get_self().value );
    auto token = _tokens.find( symcode.raw() );
    check( token != _tokens.end(), symcode.to_string() + " token not available");
    check( token->active, symcode.to_string() + " token is not active");

    if ( contract.value ) check( contract == token->contract, symcode.to_string() + " contract mismatch");
}