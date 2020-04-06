#!/bin/bash

# Config Constants

CYAN='\033[1;36m'
GREEN='\033[0;32m'
NC='\033[0m'

# load default key into local wallet
cleos wallet unlock --password $(cat ~/eosio-wallet/.pass)
cleos wallet import --private-key 5KQwrPbwdL6PhXujxW37FSSQZ1JiwsST4cqQzDeyXtP79zkvFD3

# create accounts
echo -e "${CYAN}-----------------------CREATING ACCOUNTS-----------------------${NC}"
cleos create account eosio sx EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
cleos create account eosio eosio.token EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
cleos create account eosio myaccount EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
cleos create account eosio myreferrer EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
cleos create account eosio provider EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
cleos create account eosio delphioracle EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
cleos set account permission sx active --add-code

# deploy smart contract
echo -e "${CYAN}-----------------------DEPLOYING CONTRACTS-----------------------${NC}"
cleos set contract sx ./dist sx.wasm sx.abi
cleos set contract eosio.token ./dist eosio.token.wasm eosio.token.abi
cleos set contract delphioracle ./dist delphioracle.wasm delphioracle.abi

# create tokens
echo -e "${CYAN}-----------------------CREATE TOKENS-----------------------${NC}"
cleos push action eosio.token create '["eosio", "1000000000.0000 USDA"]' -p eosio.token
cleos push action eosio.token create '["eosio", "1000000000.0000 USDB"]' -p eosio.token
cleos push action eosio.token create '["eosio", "1000000000.00000000 USDC"]' -p eosio.token
cleos push action eosio.token create '["eosio", "1000000000.00000000 BTC"]' -p eosio.token
cleos push action eosio.token create '["eosio", "1000000000.0000 EOS"]' -p eosio.token

cleos push action eosio.token issue '["eosio", "1000000000.0000 USDA", "init"]' -p eosio
cleos push action eosio.token issue '["eosio", "1000000000.0000 USDB", "init"]' -p eosio
cleos push action eosio.token issue '["eosio", "1000000000.00000000 USDC", "init"]' -p eosio
cleos push action eosio.token issue '["eosio", "1000000000.00000000 BTC", "init"]' -p eosio
cleos push action eosio.token issue '["eosio", "1000000000.0000 EOS", "init"]' -p eosio

cleos transfer eosio provider "10000000.0000 USDA"
cleos transfer eosio provider "10000000.0000 USDB"
cleos transfer eosio provider "10000000.00000000 USDC"
cleos transfer eosio provider "10000000.00000000 BTC"
cleos transfer eosio provider "10000000.0000 EOS"

cleos transfer eosio myaccount "10000000.0000 USDA"
cleos transfer eosio myaccount "10000000.0000 USDB"
cleos transfer eosio myaccount "10000000.00000000 USDC"
cleos transfer eosio myaccount "10000000.00000000 BTC"
cleos transfer eosio myaccount "10000000.0000 EOS"

echo -e "${GREEN}--> Done${NC}"
