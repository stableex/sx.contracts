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
        // check(row.balance.amount >= 0, "balance overdrawn balance");
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

double swapSx::get_ratio( const symbol_code symcode )
{
    // calculate ratio between depth & balance
    swapSx::tokens_table _tokens( get_self(), get_self().value );
    auto token = _tokens.get( symcode.raw(), "[symcode] token does not exist");
    const asset balance = swapSx::get_balance( symcode );
    return static_cast<double>(balance.amount) / token.depth.amount;
}

// void swapSx::check_max_ratio( const symbol_code symcode )
// {
//     const asset balance = get_balance( symcode );
//     const asset depth = get_depth( symcode );

//     check( static_cast<double>(balance.amount) / depth.amount <= 5, symcode.to_string() + " balance/depth ratio cannot exceed 500%" );
// }

void swapSx::check_min_balance( const asset out )
{
    const asset balance = swapSx::get_balance( out.symbol.code() );
    const asset remaining = balance - out;

    check( remaining.amount >= 0, out.symbol.code().to_string() + " insufficient remaining balance" );
}

// void swapSx::check_min_ratio( const asset out )
// {
//     const asset balance = get_balance( out.symbol.code() );
//     // const asset depth = get_depth( out.symbol.code() );
//     const asset remaining = balance - out;

//     check( remaining.amount >= 0, out.symbol.code().to_string() + " insufficient remaining balance" );
//     // check( static_cast<double>(remaining.amount) / depth.amount >= 0.2, out.symbol.code().to_string() + " balance/depth ratio must be above 20%" );
// }

void swapSx::check_is_active( const symbol_code symcode, const name contract )
{
    swapSx::tokens_table _tokens( get_self(), get_self().value );
    auto token = _tokens.find( symcode.raw() );
    check( token != _tokens.end(), symcode.to_string() + " token not available");

    if ( contract.value ) check( contract == token->contract, symcode.to_string() + " contract mismatch");
}