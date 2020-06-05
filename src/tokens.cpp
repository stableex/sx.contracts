// @static
name stable::get_contract( const name contract, const symbol_code symcode )
{
    return get_extended_symbol( contract, symcode ).get_contract();
}

// @static
symbol stable::get_symbol( const name contract, const symbol_code symcode )
{
    return get_extended_symbol( contract, symcode ).get_symbol();
}

// @static
extended_symbol stable::get_extended_symbol( const name contract, const symbol_code symcode )
{
    stable::tokens _tokens( contract, contract.value );
    auto token = _tokens.find( symcode.raw() );
    check( token != _tokens.end(), symcode.to_string() + " cannot find token");
    return extended_symbol{ token->sym, token->contract };
}

asset stable::get_balance( const symbol_code symcode )
{
    return token::get_balance( stable::get_contract( get_self(), symcode ), get_self(), symcode );
}

void stable::set_balance( const symbol_code symcode )
{
    stable::tokens _tokens( get_self(), get_self().value );
    auto itr = _tokens.find( symcode.raw() );
    if ( itr == _tokens.end() ) return;

    _tokens.modify( itr, same_payer, [&]( auto & row ) {
        row.balance = stable::get_balance( symcode );
    });
}

void stable::add_depth( const asset quantity )
{
    stable::tokens _tokens( get_self(), get_self().value );
    auto itr = _tokens.find( quantity.symbol.code().raw() );
    if ( itr == _tokens.end() ) return;

    _tokens.modify( itr, same_payer, [&]( auto & row ) {
        row.depth += quantity;
    });
}

void stable::sub_depth( const asset quantity )
{
    stable::tokens _tokens( get_self(), get_self().value );
    auto itr = _tokens.find( quantity.symbol.code().raw() );
    if ( itr == _tokens.end() ) return;

    _tokens.modify( itr, same_payer, [&]( auto & row ) {
        row.depth -= quantity;
        check(row.depth.amount >= 0, "depth overdrawn balance");
    });
}

double stable::get_ratio( const symbol_code symcode )
{
    // calculate ratio between depth & balance
    stable::tokens _tokens( get_self(), get_self().value );
    auto token = _tokens.get( symcode.raw(), "[symcode] token does not exist");
    const asset balance = stable::get_balance( symcode );
    return static_cast<double>(balance.amount) / token.depth.amount;
}

asset stable::get_depth( const symbol_code symcode )
{
    stable::tokens _tokens( get_self(), get_self().value );
    auto token = _tokens.get( symcode.raw(), "[symcode] token does not exist");
    return token.depth;
}

void stable::check_max_ratio( const symbol_code symcode )
{
    const asset balance = get_balance( symcode );
    const asset depth = get_depth( symcode );

    check( static_cast<double>(balance.amount) / depth.amount <= 5, symcode.to_string() + " balance/depth ratio cannot exceed 500%" );
}

void stable::check_min_ratio( const asset out )
{
    const asset balance = get_balance( out.symbol.code() );
    const asset depth = get_depth( out.symbol.code() );
    const asset remaining = balance - out;

    check( static_cast<double>(remaining.amount) / depth.amount >= 0.2, out.symbol.code().to_string() + " balance/depth ratio must be above 20%" );
}

void stable::check_is_active( const symbol_code symcode, const name contract )
{
    stable::tokens _tokens( get_self(), get_self().value );
    auto token = _tokens.find( symcode.raw() );
    check( token != _tokens.end(), symcode.to_string() + " token not available");

    if ( contract.value ) check( contract == token->contract, symcode.to_string() + " contract mismatch");
}