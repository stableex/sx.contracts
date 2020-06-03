#!/bin/bash

# build
eosio-cpp stable.sx.cpp -I include

# unlock wallet
cleos wallet unlock --password $(cat ~/eosio-wallet/.pass)

# create accounts
cleos create account eosio stable.sx EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
cleos create account eosio eosio.token EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
cleos create account eosio myaccount EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV

# deploy
cleos set contract stable.sx . stable.sx.wasm stable.sx.abi
cleos set contract eosio.token . eosio.token.wasm eosio.token.abi

# permission
cleos set account permission stable.sx active --add-code

# create USDT token
cleos push action eosio.token create '["eosio", "100000.0000 USDT"]' -p eosio.token
cleos push action eosio.token issue '["eosio", "5000.0000 USDT", "init"]' -p eosio
cleos transfer eosio myaccount "5000.0000 USDT" "init"

# create EOSDT token
cleos push action eosio.token create '["eosio", "100000.000000000 EOSDT"]' -p eosio.token
cleos push action eosio.token issue '["eosio", "5000.000000000 EOSDT", "init"]' -p eosio
cleos transfer eosio myaccount "5000.000000000 EOSDT" "init"
