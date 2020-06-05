// @contract
[[eosio::action]]
void stable::receipt( const name owner, const name action, const list<asset> assets )
{
    require_auth( get_self() );
    require_recipient( owner );

    // update final balances
    for (const auto quantity : assets ) {
        set_balance( quantity.symbol.code() );
    }
}

void stable::send_receipt( const name owner, const name action, const list<asset> assets )
{
    receipt_action receipt( get_self(), { get_self(), "active"_n });
    receipt.send( owner, action, assets );
}