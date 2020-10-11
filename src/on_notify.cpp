[[eosio::on_notify("*::transfer")]]
void swapSx::on_transfer( const name from, const name to, const asset quantity, const string memo )
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
    swapSx::settings _settings( get_self(), get_self().value );
    check( _settings.exists(), "contract is currently disabled for maintenance");
    auto settings = _settings.get();

    // validate input
    const symbol_code in_symcode = quantity.symbol.code();
    const symbol_code out_symcode = parse_memo_symcode( memo );
    check_is_active( in_symcode, get_first_receiver() );
    check_is_active( out_symcode, name{} );
    check( in_symcode != out_symcode, in_symcode.to_string() + " symbol code cannot be the same as quantity");

    // calculate rates
    const asset reserve_in = get_upper( in_symcode );
    const asset reserve_out = get_upper( out_symcode );
    const asset amount_out = uniswap::get_amount_out( quantity, reserve_in, reserve_out, settings.fee );

    // approximate fee
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

    // push log
    if ( from != "network.sx"_n) {
        swapSx::swaplog_action swaplog( get_self(), { get_self(), "active"_n });
        swaplog.send( from, quantity, amount_out, fee );
    }
}