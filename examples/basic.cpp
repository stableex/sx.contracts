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
    void init( const asset quantity, const symbol_code symcode )
    {
        // pre-calculate rate
        const asset rate = sx::swap::get_amount_out("swap.sx"_n, quantity, symcode);
        print("rate: " + rate.to_string() );

        // swap
        token::transfer_action transfer( "eosio.token"_n, { get_self(), "active"_n });
        transfer.send( get_self(), "swap.sx"_n, quantity, symcode.to_string() );
    }
};