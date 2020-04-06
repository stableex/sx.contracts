#!/usr/bin/env bash

mkdir -p dist

GREEN='\033[0;32m'
NC='\033[0m'

# sx
echo -e "${GREEN}Compiling sx...${NC}"
eosio-cpp -abigen \
  ./src/sx.cpp \
  -contract sx \
  -R ./ricardian \
  -o ./dist/sx.wasm \
  -I ./include \
  -I ./external/eosio.token/include \
  -I ./external/delphioracle/include

# # eosio.token
# echo -e "${GREEN}Compiling eosio.token...${NC}"
# eosio-cpp -abigen \
#   external/eosio.token/src/eosio.token.cpp \
#   -contract eosio.token \
#   -o ./dist/eosio.token.wasm \
#   -I external/eosio.token/include

# # delphioracle
# echo -e "${GREEN}Compiling delphioracle...${NC}"
# eosio-cpp -abigen \
#   external/delphioracle/src/delphioracle.cpp \
#   -contract delphioracle \
#   -o ./dist/delphioracle.wasm \
#   -I external/delphioracle/include
