// @user
[[eosio::action]]
void sx::claim( const name owner, const bool staked )
{
    require_auth( owner );
    claim_pools();

    list<asset> assets;
    if ( staked ) assets = claim_staked( owner );
    else assets = claim_transfer_out( owner );

    // prevent claiming nothing
    check( !assets.empty(), "nothing to claim" );

    // receipts
    send_receipt( owner, "claim"_n, assets );
    if ( staked ) send_receipt( owner, "stake"_n, assets );
}


list<asset> sx::claim_staked( const name owner )
{
    // withdraw any available balances from owner
    list<asset> proceeds;

    const auto account = _proceeds.find( owner.value );
    if ( account == _proceeds.end() ) return proceeds;

    // collect all `proceeds` from account
    for ( const auto [ symcode, balance ] : account->balances ) {
        if ( balance.amount > 0 ) {
            proceeds.push_back( balance );
        }
    }

    // substract proceeds from account and move to balance
    for ( const asset proceed : proceeds ) {
        sub_proceeds_owner( owner, proceed );
        add_balance( owner, proceed );
        add_depth( proceed );
    }
    return proceeds;
}
