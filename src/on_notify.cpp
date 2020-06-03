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

    // add/remove liquidity depth
    if ( from == "sx"_n ) return add_depth( quantity );
    if ( to == "sx"_n ) return sub_depth( quantity );

    // ignore transfers
    if ( to != get_self() ) return;
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
    check_max_ratio( in_symcode );

    // calculate rates
    const asset fee = calculate_fee( quantity );
    const asset out = double_to_asset( calculate_out( quantity - fee, out_symcode ), get_symbol( out_symcode ) );

    // validate output
    check_min_ratio( out );
    check( out.amount > 0, "quantity must be higher");

    // send transfers
    self_transfer( from, out, "convert" );
    self_transfer( "fee.sx"_n, fee, "fee" );

    // post transfer
    update_volume( vector<asset>{ quantity, out }, fee );
    set_balance( asset{ 0, quantity.symbol } );
    set_balance( -out );
}