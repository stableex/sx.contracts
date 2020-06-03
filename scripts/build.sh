#!/bin/bash

eosio-cpp stable.sx.cpp -I include
cleos set contract stable.sx . stable.sx.wasm stable.sx.abi
