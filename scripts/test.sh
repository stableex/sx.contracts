#!/bin/bash

# settings
cleos push action stable.sx setparams '[{"fee": 20, "amplifier": 100}]' -p stable.sx

# add initial liquidity (added manually by including ignored memo)
cleos transfer sx stable.sx "1000.0000 USDT" ""
cleos transfer sx stable.sx "1005.000000000 EOSDT" ""

# tokens
cleos push action stable.sx token '["USDT", "eosio.token"]' -p stable.sx
cleos push action stable.sx token '["EOSDT", "eosio.token"]' -p stable.sx

# convert
cleos -v transfer myaccount stable.sx "100.0000 USDT" "EOSDT"
cleos -v transfer myaccount stable.sx "100.000000000 EOSDT" "USDT"
