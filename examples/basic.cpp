#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>

#include <eosio.token/eosio.token.hpp>

#include "../swap.sx.hpp"

using namespace eosio;
using namespace std;

class [[eosio::contract]] basic : public contract {

public:
	using contract::contract;

	[[eosio::action]]
	void swap( const asset quantity )
	{
		const asset quantity = asset{10000, symbol{"EOS", 4}};
        const symbol_code symcode = symbol_code{"USDT"};
        const asset rate = swapSx::get_rate("swap.sx"_n, quantity, symcode);
        //=> "2.7712 USDT"

        // swap
        token::transfer_action transfer( "eosio.token"_n, { get_self(), "active"_n });
		transfer.send( get_self(), from, quantity, rate.to_string() );
	}
};