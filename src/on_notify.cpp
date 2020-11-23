[[eosio::on_notify("*::transfer")]]
void sx::swap::on_transfer( const name from, const name to, const asset quantity, const string memo )
{
    // authenticate incoming `from` account
    require_auth( from );

    // ignore transfers
    const set<name> ignore = set<name>{
        "eosio.ram"_n,
    };

    // ignore transfers
    if ( memo == get_self().to_string() || to != get_self() || ignore.find( from ) != ignore.end() ) {
        set_reserve( quantity.symbol.code() );
        return;
    }

    // check if contract maintenance is ongoing
    sx::swap::settings _settings( get_self(), get_self().value );
    check( _settings.exists(), "contract is currently disabled for maintenance");
    auto settings = _settings.get();

    // validate input
    const symbol_code in_symcode = quantity.symbol.code();
    const symbol_code out_symcode = parse_memo_symcode( memo );
    check_is_active( in_symcode, get_first_receiver() );
    check_is_active( out_symcode, name{} );
    check( in_symcode != out_symcode, in_symcode.to_string() + " symbol code cannot be the same as quantity");

    // calculate rates
    const asset amount_out = sx::swap::get_amount_out( get_self(), quantity, out_symcode );

    // calculate fee
    const asset fee = quantity * settings.fee / 10000;

    // validate output
    check( amount_out.amount > 0, "quantity must be higher");
    check_remaining_balance( amount_out );

    // send transfers
    self_transfer( from, amount_out, "convert" );

    // update balances `on_notify` inline transaction
    // prevents re-entry exploit
    sub_balance( amount_out );
    add_balance( quantity );

    // set virtual reserves - not used for price logic
    // eliminates having to calculate `get_upper` for each token
    set_virtual_reserve( in_symcode );
    set_virtual_reserve( out_symcode );

    // update reserve for input quantity
    set_reserve( quantity.symbol.code() );

    // push log
    if ( from != "network.sx"_n && is_account("stats.sx"_n) ) {
        sx::stats::swaplog_action swaplog( "stats.sx"_n, { get_self(), "active"_n });
        swaplog.send( get_self(), from, quantity, amount_out, fee );
    }
}