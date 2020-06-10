[[eosio::on_notify("*::transfer")]]
void swapSx::on_transfer( const name from, const name to, const asset quantity, const string memo )
{
    // authenticate incoming `from` account
    require_auth( from );

    // ignore transfers
    const set<name> ignore = set<name>{
        // EOSIO system accounts
        "eosio.stake"_n,
        "eosio.names"_n,
        "eosio.ram"_n,
        "eosio.rex"_n,
        "eosio"_n,
    };

    // update balances (post convert transfer, triggered by `on_notify`)
    if ( from == get_self() && ( memo == "convert" || memo == "fee") ) {
        set_balance( quantity.symbol.code() );
        update_spot_prices( symbol_code{"USDT"} );
    }

    // add/remove liquidity depth (must be sent using `sx` account)
    if ( from == "sx"_n || to == "sx"_n ) {
        set_balance( quantity.symbol.code() );
        update_spot_prices( symbol_code{"USDT"} );
    }
    if ( from == "sx"_n ) return add_depth( quantity );
    if ( to == "sx"_n ) return sub_depth( quantity );

    // prevent invalid transfers
    if ( from == get_self() ) check( memo == "convert" || memo == "fee", "invalid transfer");

    // ignore transfers
    if ( to != get_self() ) return;
    if ( ignore.find( from ) != ignore.end() ) return;

    // check if contract maintenance is ongoing
    swapSx::settings _settings( get_self(), get_self().value );
    check( _settings.exists(), "contract is currently disabled for maintenance");

    // validate input
    const symbol_code in_symcode = quantity.symbol.code();
    const symbol_code out_symcode = parse_memo_symcode( memo );
    check_is_active( in_symcode, get_first_receiver() );
    check_is_active( out_symcode, name{} );
    check( in_symcode != out_symcode, in_symcode.to_string() + " symbol code cannot be the same as quantity");
    check_max_ratio( in_symcode );

    // calculate rates
    const asset fee = swapSx::get_fee( get_self(), quantity );
    const asset rate = swapSx::get_rate( get_self(), quantity, out_symcode );

    // validate output
    check_min_ratio( rate );
    check( rate.amount > 0, "quantity must be higher");

    // send transfers
    self_transfer( from, rate, "convert" );
    self_transfer( "fee.sx"_n, fee, "fee" );

    // post transfer
    update_volume( vector<asset>{ quantity, rate }, fee );
    set_balance( quantity.symbol.code() );
}