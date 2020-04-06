[[eosio::on_notify("*::transfer")]]
void sx::on_transfer( const name    from,
                      const name    to,
                      const asset   quantity,
                      const string  memo )
{
    // authenticate incoming `from` account
    require_auth( from );

    // inputs
    const name contract = get_first_receiver();
    const symbol_code in_symcode = quantity.symbol.code();

    // on eosio.token EOS token transfer
    if ( contract == "eosio.token"_n && quantity.symbol == symbol{"EOS", 4} ) {
        // accept receiving EOS from system contracts
        if ( from == "eosio.ram"_n || from == "eosio.stake"_n || from == "eosio.names"_n ) return;
    }

    // Only monitor incoming transfers to get_self() account
    if ( to != get_self() ) return;

    // ignore transfers
    if ( memo == "ignore" ) return;

    // settings
    check( _settings.exists(), "contract is not initialized");
    check( !_settings.get().paused, "contract is currently paused for maintenance");

    // validation of incoming transfer
    check_pool_contract( in_symcode, contract );

    // parse memo
    // schema: "{symbol_code}"
    vector<string> tokens = split( memo, "," );
    const symbol_code out_symcode = parse_memo_symcode( tokens );

    // convert if memo includes symbol code (ex: USDT)
    if ( out_symcode.raw() > 0 ) {
        const asset out = convert_out( from, quantity, out_symcode );
        send_receipt( from, "convert"_n, list<asset>{ quantity, out });
    }
    // auto-stake by default
    else {
        claim_pools();
        add_balance( from, quantity );
        add_depth( quantity );
        check_min_staked( from, in_symcode );
        send_receipt( from, "stake"_n, list<asset>{ quantity });
    }
}
