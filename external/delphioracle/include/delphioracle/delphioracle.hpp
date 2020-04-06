#pragma once

#include <eosio/eosio.hpp>
#include <eosio/system.hpp>
#include <eosio/time.hpp>

#include <string>

using namespace eosio;
using namespace std;

class [[eosio::contract("delphioracle")]] delphioracle : public contract {
public:
    using contract::contract;

    [[eosio::action]]
    void set( const eosio::name scope, const uint64_t value );

    //Holds the last datapoints_count datapoints from qualified oracles
    struct [[eosio::table]] datapoints {
        uint64_t id;
        name owner;
        uint64_t value;
        uint64_t median;
        eosio::time_point timestamp;

        uint64_t primary_key() const {return id;}
        uint64_t by_timestamp() const {return timestamp.elapsed.to_seconds();}
        uint64_t by_value() const {return value;}
    };

    typedef eosio::multi_index<"datapoints"_n, datapoints,
        indexed_by<"value"_n, const_mem_fun<datapoints, uint64_t, &datapoints::by_value>>,
        indexed_by<"timestamp"_n, const_mem_fun<datapoints, uint64_t, &datapoints::by_timestamp>>> datapointstable;
};
