#!/bin/bash

# unlock wallet
cleos wallet unlock --password $(cat ~/eosio-wallet/.pass)

# create accounts
cleos create account eosio sx EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
cleos create account eosio swap.sx EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
cleos create account eosio fee.sx EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
cleos create account eosio log.sx EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
cleos create account eosio registry.sx EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
cleos create account eosio eosio.token EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
cleos create account eosio myaccount EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
cleos create account eosio basic EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
cleos create account eosio entry EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV

# deploy
cleos set contract swap.sx . swap.sx.wasm swap.sx.abi
cleos set contract entry . entry.wasm entry.abi
cleos set contract basic . basic.wasm basic.abi
cleos set contract eosio.token . eosio.token.wasm eosio.token.abi

# permission
cleos set account permission swap.sx active --add-code
cleos set account permission basic active --add-code
cleos set account permission entry active --add-code

# create A token
cleos push action eosio.token create '["eosio", "100000000.0000 A"]' -p eosio.token
cleos push action eosio.token issue '["eosio", "5000000.0000 A", "init"]' -p eosio
cleos transfer eosio myaccount "50000.0000 A" "init"
cleos transfer eosio sx "50000.0000 A" "init"
cleos transfer eosio entry "100.0000 A" "init"
cleos transfer eosio basic "100.0000 A" "init"

# create B token
cleos push action eosio.token create '["eosio", "100000000.000000000 B"]' -p eosio.token
cleos push action eosio.token issue '["eosio", "50000000.000000000 B", "init"]' -p eosio
cleos transfer eosio myaccount "50000.000000000 B" "init"
cleos transfer eosio sx "50000.000000000 B" "init"

# create C token
cleos push action eosio.token create '["eosio", "100000000.0000 C"]' -p eosio.token
cleos push action eosio.token issue '["eosio", "5000000.0000 C", "init"]' -p eosio
cleos transfer eosio myaccount "50000.0000 C" "init"
cleos transfer eosio sx "50000.0000 C" "init"
cleos transfer eosio basic "100.0000 C" "init"