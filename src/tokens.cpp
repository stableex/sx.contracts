void sx::swap::add_balance( const asset quantity )
{
    sx::swap::tokens _tokens( get_self(), get_self().value );
    auto itr = _tokens.find( quantity.symbol.code().raw() );
    if ( itr == _tokens.end() ) return;

    _tokens.modify( itr, same_payer, [&]( auto & row ) {
        row.balance += quantity;
    });
}

void sx::swap::sub_balance( const asset quantity )
{
    sx::swap::tokens _tokens( get_self(), get_self().value );
    auto itr = _tokens.find( quantity.symbol.code().raw() );
    if ( itr == _tokens.end() ) return;

    _tokens.modify( itr, same_payer, [&]( auto & row ) {
        row.balance -= quantity;
        // `balance` (initial pegged balance) can go in negative
        // price formula will adjusts accordingly
    });
}

void sx::swap::set_reserve( const symbol_code symcode )
{
    sx::swap::tokens _tokens( get_self(), get_self().value );
    auto itr = _tokens.find( symcode.raw() );
    if ( itr == _tokens.end() ) return;

    _tokens.modify( itr, same_payer, [&]( auto & row ) {
        row.reserve = token::get_balance( get_contract( get_self(), symcode ), get_self(), symcode );
    });
}

bool sx::swap::is_token_exists( const symbol_code symcode )
{
    sx::swap::tokens _tokens( get_self(), get_self().value );
    return ( _tokens.find( symcode.raw() ) != _tokens.end() );
}

void sx::swap::check_remaining_balance( const asset out )
{
    const symbol_code symcode = out.symbol.code();
    const name contract = get_contract( get_self(), symcode );
    const asset balance = token::get_balance( contract, get_self(), symcode );
    const asset remaining = balance - out;

    check( remaining.amount >= 0, get_self().to_string() + " has insufficient balance to convert " + out.to_string() );
}

void sx::swap::check_is_active( const symbol_code symcode, const name contract )
{
    sx::swap::tokens _tokens( get_self(), get_self().value );
    auto token = _tokens.find( symcode.raw() );
    check( token != _tokens.end(), symcode.to_string() + " token not available");

    if ( contract.value ) check( contract == token->contract, symcode.to_string() + " contract mismatch");
}