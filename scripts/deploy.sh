#!/bin/bash

# unlock wallet
cleos wallet unlock --password $(cat ~/eosio-wallet/.pass)

# create accounts
cleos create account eosio sx EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
cleos create account eosio stable.sx EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
cleos create account eosio fee.sx EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
cleos create account eosio eosio.token EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
cleos create account eosio myaccount EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV

# deploy
cleos set contract stable.sx . stable.sx.wasm stable.sx.abi
cleos set contract eosio.token . eosio.token.wasm eosio.token.abi

# permission
cleos set account permission stable.sx active --add-code

# create USDT token
cleos push action eosio.token create '["eosio", "100000000.0000 USDT"]' -p eosio.token
cleos push action eosio.token issue '["eosio", "5000000.0000 USDT", "init"]' -p eosio
cleos transfer eosio myaccount "50000.0000 USDT" "init"
cleos transfer eosio sx "50000.0000 USDT" "init"

# create EOSDT token
cleos push action eosio.token create '["eosio", "100000000.000000000 EOSDT"]' -p eosio.token
cleos push action eosio.token issue '["eosio", "50000000.000000000 EOSDT", "init"]' -p eosio
cleos transfer eosio myaccount "50000.000000000 EOSDT" "init"
cleos transfer eosio sx "50000.000000000 EOSDT" "init"

# create EOS token
cleos push action eosio.token create '["eosio", "100000000.0000 EOS"]' -p eosio.token
cleos push action eosio.token issue '["eosio", "5000000.0000 EOS", "init"]' -p eosio
cleos transfer eosio myaccount "50000.0000 EOS" "init"
cleos transfer eosio sx "50000.0000 EOS" "init"