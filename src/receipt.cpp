// @contract
[[eosio::action]]
void sx::receipt( const name owner, const name action, const list<asset> assets )
{
    require_auth( get_self() );
    require_recipient( owner );

    // update final balances, depth & ratio of pools
    for (const auto asset : assets ) {
        const symbol_code symcode = asset.symbol.code();
        update_pool_ratio( symcode );
    }
    clear_empty_balances( owner );
}

void sx::send_receipt( const name owner, const name action, const list<asset> assets )
{
    receipt_action receipt( get_self(), { get_self(), "active"_n });
    receipt.send( owner, action, assets );
}
