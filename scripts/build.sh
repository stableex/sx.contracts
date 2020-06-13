#!/bin/bash

eosio-cpp swap.sx.cpp -I include
cleos set contract swap.sx . swap.sx.wasm swap.sx.abi

eosio-cpp examples/entry.cpp -I include
cleos set contract entry . entry.wasm entry.abi
