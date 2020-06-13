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

void swapSx::set_balance( const symbol_code symcode )
{
    swapSx::tokens_table _tokens( get_self(), get_self().value );
    auto itr = _tokens.find( symcode.raw() );
    if ( itr == _tokens.end() ) return;

    // get active token balance of self
    const name contract = swapSx::get_contract( get_self(), symcode );
    const asset balance = token::get_balance( contract, get_self(), symcode );

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
        check(row.balance.amount >= 0, "balance overdrawn balance");
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