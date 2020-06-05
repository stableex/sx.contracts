#!/bin/bash

# settings
cleos push action stable.sx setparams '[{"fee": 20, "amplifier": 20}]' -p stable.sx

# add initial liquidity (added manually by including ignored memo)
cleos transfer sx stable.sx "250.0000 USDT" ""
cleos transfer sx stable.sx "250.000000000 EOSDT" ""
cleos transfer sx stable.sx "92.6000 EOS" ""

# tokens
cleos push action stable.sx token '["EOS", "eosio.token"]' -p stable.sx
cleos push action stable.sx token '["USDT", "eosio.token"]' -p stable.sx
cleos push action stable.sx token '["EOSDT", "eosio.token"]' -p stable.sx

# convert
cleos -v transfer myaccount stable.sx "100.0000 USDT" "EOSDT"
cleos -v transfer myaccount stable.sx "100.000000000 EOSDT" "USDT"
