#include <sx/sx.hpp>
#include <eosio.token/eosio.token.hpp>
#include <delphioracle/delphioracle.hpp>

// core
#include "on_notify.cpp"
#include "settings.cpp"
#include "accounts.cpp"
#include "pools.cpp"
#include "utils.cpp"
#include "convert.cpp"
#include "volume.cpp"
#include "claim.cpp"
#include "receipt.cpp"
#include "proceeds.cpp"
#include "prices.cpp"

// maintenance
#include "maintenance/migrate.cpp"
#include "maintenance/clean.cpp"
