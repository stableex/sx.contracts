#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>

#include <eosio.token/eosio.token.hpp>

#include "../swap.sx.hpp"

using namespace eosio;
using namespace std;

class [[eosio::contract]] entry : public contract {

public:
	using contract::contract;

    struct [[eosio::table("global")]] global_row {
        int      attempts = 0;
    };
    typedef eosio::singleton< "global"_n, global_row > global_table;

	[[eosio::action]]
	void init( const asset quantity )
	{
        token::transfer_action transfer( "eosio.token"_n, { get_self(), "active"_n });
		transfer.send( get_self(), "swap.sx"_n, quantity, "USDT" );
	}

	[[eosio::on_notify("eosio.token::transfer")]]
	void on_transfer( const name from, const name to, asset quantity, string memo )
	{
        if ( from != "swap.sx"_n ) return;
		global_table _global( get_self(), get_self().value );
		auto global = _global.get_or_create( get_self() );

		// stop re-entry
		if ( global.attempts > 1 ) return;

		// log
        print( "quantity:" + quantity.to_string() + "\n");
		print( "attempts:" + to_string( global.attempts ) + "\n");

		// update number of attempts
		global.attempts += 1;
		_global.set( global, get_self() );

        // re-rentry with incoming transfer
        memo = quantity.symbol.code() == symbol_code{"EOS"} ? "USDT" : "EOS";
		token::transfer_action transfer( "eosio.token"_n, { get_self(), "active"_n });
		transfer.send( get_self(), "swap.sx"_n, quantity, memo );
	}
};