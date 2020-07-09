asset swapSx::get_balance( const symbol_code symcode )
{
    swapSx::tokens_table _tokens( get_self(), get_self().value );
    auto token = _tokens.get( symcode.raw(), "[symcode] token does not exist");
    return token.balance;
}

asset swapSx::get_depth( const symbol_code symcode )
{
    swapSx::tokens_table _tokens( get_self(), get_self().value );
    auto token = _tokens.get( symcode.raw(), "[symcode] token does not exist");
    return token.depth;
}

void swapSx::set_balance( const asset balance )
{
    check(balance.amount >= 0, balance.to_string() + " balance must be positive");

    swapSx::tokens_table _tokens( get_self(), get_self().value );
    auto itr = _tokens.find( balance.symbol.code().raw() );
    if ( itr == _tokens.end() ) return;

    // ignore setting balance if the same
    if ( itr->balance == balance ) return;

    _tokens.modify( itr, same_payer, [&]( auto & row ) {
        row.balance = balance;
    });
}

void swapSx::add_balance( const asset quantity )
{
    swapSx::tokens_table _tokens( get_self(), get_self().value );
    auto itr = _tokens.find( quantity.symbol.code().raw() );
    if ( itr == _tokens.end() ) return;

    _tokens.modify( itr, same_payer, [&]( auto & row ) {
        row.balance += quantity;
    });
}

void swapSx::sub_balance( const asset quantity )
{
    swapSx::tokens_table _tokens( get_self(), get_self().value );
    auto itr = _tokens.find( quantity.symbol.code().raw() );
    if ( itr == _tokens.end() ) return;

    _tokens.modify( itr, same_payer, [&]( auto & row ) {
        row.balance -= quantity;
        // balance (initial pegged balance) can go in negative
        // price formula will adjusts accordingly
    });
}

void swapSx::add_depth( const asset quantity )
{
    swapSx::tokens_table _tokens( get_self(), get_self().value );
    auto itr = _tokens.find( quantity.symbol.code().raw() );
    if ( itr == _tokens.end() ) return;

    _tokens.modify( itr, same_payer, [&]( auto & row ) {
        row.depth += quantity;
    });
}

void swapSx::sub_depth( const asset quantity )
{
    swapSx::tokens_table _tokens( get_self(), get_self().value );
    auto itr = _tokens.find( quantity.symbol.code().raw() );
    if ( itr == _tokens.end() ) return;

    _tokens.modify( itr, same_payer, [&]( auto & row ) {
        row.depth -= quantity;
        check(row.depth.amount >= 0, "depth overdrawn balance");
    });
}

void swapSx::set_depth( const asset depth )
{
    check(depth.amount >= 0, "depth must be positive");

    swapSx::tokens_table _tokens( get_self(), get_self().value );
    auto itr = _tokens.find( depth.symbol.code().raw() );
    check( itr != _tokens.end(), "[symcode] token does not exist");

    // no change in depth
    if ( itr->depth == depth ) return;

    _tokens.modify( itr, same_payer, [&]( auto & row ) {
        row.depth = depth;
    });
}

bool swapSx::is_token_exists( const symbol_code symcode )
{
    swapSx::tokens_table _tokens( get_self(), get_self().value );
    return ( _tokens.find( symcode.raw() ) != _tokens.end() );
}

void swapSx::check_remaining_balance( const asset out )
{
    const symbol_code symcode = out.symbol.code();
    const name contract = get_contract( get_self(), symcode );
    const asset balance = token::get_balance( contract, get_self(), symcode );
    const asset remaining = balance - out;

    check( remaining.amount >= 0, get_self().to_string() + " has insufficient balance to convert " + out.to_string() );
}

void swapSx::check_is_active( const symbol_code symcode, const name contract )
{
    swapSx::tokens_table _tokens( get_self(), get_self().value );
    auto token = _tokens.find( symcode.raw() );
    check( token != _tokens.end(), symcode.to_string() + " token not available");

    if ( contract.value ) check( contract == token->contract, symcode.to_string() + " contract mismatch");
}