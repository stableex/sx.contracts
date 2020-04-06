void sx::sub_proceeds_owner( const name owner, const asset value )
{
    auto from = _proceeds.find( owner.value );
    check( from != _proceeds.end(), "no balance object found" );
    const symbol_code symcode = value.symbol.code();

    bool is_empty = false;
    _proceeds.modify( from, same_payer, [&]( auto& row ) {
        row.balances[ symcode ] -= value;
        check( row.balances[ symcode ].amount >= 0, "overdrawn balance" );
        if ( row.balances[ symcode ].amount <= 0) row.balances.erase( symcode );
        is_empty = row.balances.empty();
    });
}

void sx::add_proceeds_owner( const name owner, const asset value )
{
    const name ram_payer = get_self();

    auto to = _proceeds.find( owner.value );
    const symbol_code symcode = value.symbol.code();

    if ( to == _proceeds.end() ) {
        _proceeds.emplace( ram_payer, [&]( auto& row ){
            row.owner = owner;
            row.balances[symcode] = value;
        });
    } else {
        _proceeds.modify( to, same_payer, [&]( auto& row ) {
            if (row.balances[ symcode ].symbol.code() == symcode ) {
                row.balances[ symcode ] += value;
            } else {
                row.balances[ symcode ] = value;
            }
        });
    }
}
