#!/bin/bash

# settings
cleos push action swap.sx setparams '[{"fee": 30, "amplifier": 20}]' -p swap.sx

# add initial liquidity (added manually by including ignored memo)
cleos transfer sx swap.sx "250.0000 USDT" ""
cleos transfer sx swap.sx "250.000000000 EOSDT" ""
cleos transfer sx swap.sx "92.6000 EOS" ""

# tokens
cleos push action swap.sx token '["EOS", "eosio.token"]' -p swap.sx
cleos push action swap.sx token '["USDT", "eosio.token"]' -p swap.sx
cleos push action swap.sx token '["EOSDT", "eosio.token"]' -p swap.sx

# convert
cleos -v transfer myaccount swap.sx "100.0000 USDT" "EOSDT"
cleos -v transfer myaccount swap.sx "100.000000000 EOSDT" "USDT"
