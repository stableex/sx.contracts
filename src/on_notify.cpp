[[eosio::on_notify("*::transfer")]]
void stable::on_transfer( const name from, const name to, const asset quantity, const string memo )
{
    // authenticate incoming `from` account
    require_auth( from );

    // ignore transfers
    const set<name> ignore = set<name>{
        // system
        "eosio.stake"_n,
        "eosio.names"_n,
        "eosio.ram"_n,
        "eosio.rex"_n,
        "eosio"_n,
    };
    if ( to != get_self() ) return;
    if ( memo == get_self().to_string() ) return;
    if ( ignore.find( from ) != ignore.end() ) return;

    // check if contract maintenance is ongoing
    stable::settings _settings( get_self(), get_self().value );
    check( _settings.exists(), "contract is currently disabled for maintenance");

    // validate input
    const symbol_code in_symcode = quantity.symbol.code();
    const symbol_code out_symcode = parse_memo_symcode( memo );
    check_is_active( in_symcode, get_first_receiver() );
    check_is_active( out_symcode, name{} );
    check( in_symcode != out_symcode, in_symcode.to_string() + " symbol code cannot be the same as quantity");
    check_max_ratio( quantity );

    // get current balances
    const asset base = stable::get_balance( in_symcode ) - quantity;
    const asset quote = stable::get_balance( out_symcode );

    // calculate rates
    const asset fee = calculate_fee( quantity );
    const asset out = double_to_asset( calculate_out( quantity - fee, base, quote ), quote.symbol );

    // validate output
    check_min_ratio( out );
    check( out.amount > 0, "quantity must be higher");

    // send transfer
    token::transfer_action transfer( get_contract( out_symcode ), { get_self(), "active"_n });
    transfer.send( get_self(), from, out, "convert" );
}