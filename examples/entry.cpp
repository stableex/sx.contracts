#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>

#include <eosio.token/eosio.token.hpp>

using namespace eosio;
using namespace std;

class [[eosio::contract]] entry : public contract {

public:
	using contract::contract;

	[[eosio::action]]
	void init( const asset quantity )
	{
        token::transfer_action transfer( "eosio.token"_n, { get_self(), "active"_n });
		transfer.send( get_self(), "swap.sx"_n, quantity, "B" );
	}

	[[eosio::on_notify("eosio.token::transfer")]]
	void on_transfer( const name from, const name to, const asset quantity, const string memo )
	{
        if ( to != get_self() ) return;
		if ( quantity.symbol.code() != symbol_code{"B"} ) return;

        // re-rentry with incoming transfer
		token::transfer_action transfer( "eosio.token"_n, { get_self(), "active"_n });

		// split into 4 requests
		const asset partial = quantity / 4;
		transfer.send( get_self(), "swap.sx"_n, partial, "A" );
		transfer.send( get_self(), "swap.sx"_n, partial, "A" );
		transfer.send( get_self(), "swap.sx"_n, partial, "A" );
		transfer.send( get_self(), "swap.sx"_n, partial, "A" );
	}
};