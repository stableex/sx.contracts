#!/bin/bash

# settings
cleos push action swap.sx setparams '[{"fee": 20, "amplifier": 20, "base": "USDT"}]' -p swap.sx
cleos push action stable.sx setparams '[{"fee": 20, "amplifier": 100, "base": "USDT"}]' -p stable.sx

# add initial liquidity (added manually by including ignored memo)
cleos transfer sx swap.sx "2000.0000 EOS" "swap.sx"
cleos transfer sx swap.sx "5000.0000 USDT" "swap.sx"
cleos transfer sx swap.sx "5000.0000 A" "swap.sx"
cleos transfer sx swap.sx "5000.000000000 B" "swap.sx"

cleos transfer sx stable.sx "5000.0000 A" "stable.sx"
cleos transfer sx stable.sx "5000.0000 B" "stable.sx"
cleos transfer sx stable.sx "5000.0000 USDT" "stable.sx"

# tokens
cleos push action swap.sx token '["EOS", "eosio.token"]' -p swap.sx
cleos push action swap.sx token '["USDT", "eosio.token"]' -p swap.sx
cleos push action swap.sx token '["A", "eosio.token"]' -p swap.sx
cleos push action swap.sx token '["B", "eosio.token"]' -p swap.sx

cleos push action stable.sx token '["USDT", "eosio.token"]' -p stable.sx
cleos push action stable.sx token '["A", "eosio.token"]' -p stable.sx
cleos push action stable.sx token '["B", "eosio.token"]' -p stable.sx

# convert
cleos -v transfer myaccount swap.sx "100.0000 EOS" "USDT"
cleos -v transfer myaccount swap.sx "248.1327 USDT" "EOS"

# # examples - basic
# cleos push action basic init '["50.0000 A", "B"]' -p basic

# # examples - entry
# cleos push action entry init '["50.0000 A"]' -p entry
# cleos get currency balance eosio.token entry
