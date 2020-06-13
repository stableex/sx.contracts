#!/bin/bash

# settings
cleos push action swap.sx setparams '[{"fee": 50, "amplifier": 20}]' -p swap.sx

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

# trade log
cleos push action swap.sx tradelog '["myaccount", "1.0000 EOS", "2.5300 USDT", "0.0050 EOS", 2.53]' -p swap.sx

# examples - entry
cleos push action entry init '["1.0000 EOS"]' -p entry
