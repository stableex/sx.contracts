#!/bin/bash

# settings
cleos push action swap.sx setparams '[{"fee": 50, "fee_account": "fee.sx", "amplifier": 20, "base": "USDT"}]' -p swap.sx

# add initial liquidity (added manually by including ignored memo)
cleos transfer sx swap.sx "2000.0000 EOS" "swap.sx"
cleos transfer sx swap.sx "5000.0000 USDT" "swap.sx"
cleos transfer sx swap.sx "5000.0000 A" "swap.sx"
cleos transfer sx swap.sx "5000.000000000 B" "swap.sx"

# tokens
cleos push action swap.sx token '["EOS", "eosio.token"]' -p swap.sx
cleos push action swap.sx token '["USDT", "eosio.token"]' -p swap.sx
cleos push action swap.sx token '["A", "eosio.token"]' -p swap.sx
cleos push action swap.sx token '["B", "eosio.token"]' -p swap.sx

# convert
cleos -v transfer myaccount swap.sx "100.0000 EOS" "USDT"
cleos -v transfer myaccount swap.sx "248.1327 USDT" "EOS"

# # examples - basic
# cleos push action basic init '["50.0000 A", "B"]' -p basic

# # examples - entry
# cleos push action entry init '["50.0000 A"]' -p entry
# cleos get currency balance eosio.token entry
