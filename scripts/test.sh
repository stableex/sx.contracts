#!/bin/bash

# settings
cleos push action stable.sx setparams '[{"fee": 10, "amplifier": 20, "spot_prices": ["EOSDT", "USDT"], "pool": {"contract":"eosio.token", "sym": "4,SXS"}}]' -p stable.sx

# add initial liquidity (added manually by including ignored memo)
cleos transfer myaccount stable.sx "500.0000 USDT" "stable.sx"
cleos transfer myaccount stable.sx "500.000000000 EOSDT" "stable.sx"

# tokens
cleos push action stable.sx token '["USDT", "eosio.token", true]' -p stable.sx
cleos push action stable.sx token '["EOSDT", "eosio.token", true]' -p stable.sx

# convert
cleos -v transfer myaccount stable.sx "1.000000000 EOSDT" "USDT"
