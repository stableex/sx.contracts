#!/bin/bash

# set delphioracle
cleos push action delphioracle set '["eosusd", 50000]' -p delphioracle
cleos push action delphioracle set '["btcusd", 95000000]' -p delphioracle

# setparams
cleos push action sx init '[]' -p sx
cleos push action sx pause '[false]' -p sx
cleos push action sx setmin '["0.5000 USD", "0.0000 USD"]' -p sx
cleos push action sx setfee '[4, 0]' -p sx

# set pools
cleos push action sx create '["USDA", "eosio.token", "USD"]'  -p sx
cleos push action sx create '["USDB", "eosio.token", "USD"]'  -p sx
cleos push action sx create '["USDC", "eosio.token", "USD"]'  -p sx
cleos push action sx create '["EOS", "eosio.token", "EOS"]'  -p sx
cleos push action sx create '["BTC", "eosio.token", "BTC"]'  -p sx
cleos push action sx enable '["USDA", true]' -p sx
cleos push action sx enable '["USDB", true]' -p sx
cleos push action sx enable '["USDC", true]' -p sx
cleos push action sx enable '["EOS", true]' -p sx
cleos push action sx enable '["BTC", true]' -p sx

# pegged
cleos push action sx setpegged '["USDB", "0.9900 USD"]' -p sx

# set amplifiers
cleos push action sx setamplifier '["EOS", 5]'  -p sx
cleos push action sx setamplifier '["USDA", 50]'  -p sx
cleos push action sx setamplifier '["USDB", 50]'  -p sx
cleos push action sx setamplifier '["USDC", 50]'  -p sx

# connectors
cleos push action sx addconnector '["USDA", "USDB"]' -p sx
cleos push action sx addconnector '["USDB", "USDA"]' -p sx
cleos push action sx addconnector '["USDA", "USDC"]' -p sx
cleos push action sx addconnector '["USDB", "USDC"]' -p sx
cleos push action sx addconnector '["USDC", "USDA"]' -p sx
cleos push action sx addconnector '["USDC", "USDB"]' -p sx

# BTC
cleos push action sx addconnector '["USDA", "BTC"]' -p sx
cleos push action sx addconnector '["BTC", "USDA"]' -p sx
cleos push action sx addconnector '["EOS", "BTC"]' -p sx
cleos push action sx addconnector '["BTC", "EOS"]' -p sx
cleos push action sx addconnector '["BTC", "USDB"]' -p sx
cleos push action sx addconnector '["USDB", "BTC"]' -p sx

# EOS
cleos push action sx addconnector '["USDA", "EOS"]' -p sx
cleos push action sx addconnector '["USDB", "EOS"]' -p sx
cleos push action sx addconnector '["USDC", "EOS"]' -p sx
cleos push action sx addconnector '["EOS", "USDA"]' -p sx
cleos push action sx addconnector '["EOS", "USDB"]' -p sx
cleos push action sx addconnector '["EOS", "USDC"]' -p sx

# set pegged
cleos push action sx setpegged '["USDC", "0.9950 USD"]' -p sx

# stake
cleos transfer provider sx "1100.0000 USDA" ""
cleos transfer provider sx "700.0000 USDB" ""
cleos transfer provider sx "20000.0000 USDC" ""
cleos transfer provider sx "900.0000 EOS" ""
cleos transfer provider sx "0.35000000 BTC" ""

# # stake & unstake
# cleos -v transfer myaccount sx "50.0000 USDB" ""
# cleos -v transfer myaccount sx "50.0000 USDA" ""
# cleos -v push action sx unstake '["myaccount", "25.0000 USDA"]' -p myaccount

# # convert
# cleos -v transfer myaccount sx "10.0000 USDA" "USDB"
# cleos -v transfer myaccount sx "1000.0000 USDC" "USDB"
# cleos -v transfer myaccount sx "1000.0000 USDB" "USDC"

# # claim
# cleos -v push action sx claim '["provider", false]' -p provider
