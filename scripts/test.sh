#!/bin/bash

# settings
cleos push action swap.sx setparams '[{"fee": 50, "amplifier": 20}]' -p swap.sx

# add initial liquidity (added manually by including ignored memo)
cleos transfer sx swap.sx "5000.0000 A" ""
cleos transfer sx swap.sx "5000.000000000 B" ""
cleos transfer sx swap.sx "1960.7843 C" ""

# tokens
cleos push action swap.sx token '["A", "eosio.token"]' -p swap.sx
cleos push action swap.sx token '["B", "eosio.token"]' -p swap.sx
cleos push action swap.sx token '["C", "eosio.token"]' -p swap.sx

# convert
cleos -v transfer myaccount swap.sx "100.0000 A" "B"
cleos -v transfer myaccount swap.sx "100.000000000 B" "A"

# examples - basic
cleos push action basic init '["50.0000 A", "B"]' -p basic

# examples - entry
cleos push action entry init '["50.0000 A"]' -p entry
cleos get currency balance eosio.token entry

