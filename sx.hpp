#pragma once

#include <eosio/eosio.hpp>
#include <eosio/system.hpp>
#include <eosio/singleton.hpp>
#include <eosio/asset.hpp>

#include <math.h>
#include <string>

using namespace eosio;
using namespace std;

// symbol codes
static constexpr symbol_code USD = symbol_code{"USD"};
static constexpr symbol_code EOS = symbol_code{"EOS"};
static constexpr symbol_code BTC = symbol_code{"BTC"};
static constexpr symbol_code USDT = symbol_code{"USDT"};
static constexpr symbol_code EOSDT = symbol_code{"EOSDT"};

// time
static constexpr uint64_t MINUTE = 60; // 1 min
static constexpr uint64_t HOUR = 3600; // 1 hour
static constexpr uint64_t DAY = 86400; // 24 hours
static constexpr uint64_t WEEK = 604800; // 7 days
static constexpr uint64_t MONTH = 2592000; // 30 days

/**
 * ## TABLE `settings`
 *
 * - `{bool} [paused=false]` - (true/false) entire contract paused
 * - `{asset} [transaction_fee=0.00 USD]` - transaction fee paid to the contract
 * - `{int64_t} [admin_fee=0]` - trading fee towards admin contract (pips 1/100 of 1%)
 * - `{int64_t} [pool_fee=6]` - trading fees towards liquidity pool providers (pips 1/100 of 1%)
 * - `{asset} [min_convert=1.00 USD]` - minimum amount to convert
 * - `{asset} [min_staked=1.00 USD]` - minimum amount to stake
 *
 * ### example
 *
 * ```json
 * {
 *   "min_pool_threshold": 2000,
 *   "paused": false,
 *   "transaction_fee": "0.00 USD",
 *   "admin_fee": 0,
 *   "pool_fee": 6,
 *   "min_convert": "1.00 USD",
 *   "min_staked": "1.00 USD"
 * }
 * ```
 */
struct [[eosio::table("settings"), eosio::contract("sx")]] sx_parameters {
    bool            paused = false;
    asset           transaction_fee = asset{0'00, symbol{"USD", 4}};
    int64_t         admin_fee = 0;
    int64_t         pool_fee = 6;
    asset           min_convert = asset{1'00, symbol{"USD", 4}};
    asset           min_staked = asset{1'00, symbol{"USD", 4}};
};
typedef eosio::singleton< "settings"_n, sx_parameters> settings_table;

/**
 * ## TABLE `pools`
 *
 * - `{extended_symbol} id` - extended symbol ID
 * - `{asset} [balance=0.0000 USD]` - remaining balance
 * - `{asset} [depth=0.0000 USD]` - liquidity depth
 * - `{uint64_t} [ratio=100'00]` - ratio between balance & depth (pips 1/100 of 1%)
 * - `{asset} [proceeds=0.0000 USD]` - accumulated trading fee proceeds
 * - `{int64_t} [amplifier=1]` - liquidity pool amplifier (ex: 20x multiplier)
 * - `{symbol_code} [type=USD]` - liquidity type (ex: USD, EUR, CNY)
 * - `{asset} [pegged="1.0000 USD"]` - pegged price in USD
 * - `{set<symbol_code>} [connectors=[]]` - liquidity connectors
 * - `{bool} [enabled=false]` - enable trading (true/false)
 * - `{map<name, string>} [metadata_json={}]` - a sorted container of <key, value>
 *
 * ### example
 *
 * ```json
 * {
 *   "id": {
 *     "symbol": "USDT,4",
 *     "contract": "tethertether"
 *   },
 *   "balance": "10000.0000 USDT",
 *   "depth": "10000.0000 USDT",
 *   "ratio": 9988,
 *   "proceeds": "0.0000 USDT",
 *   "amplifier": 1,
 *   "type": "USD",
 *   "pegged": "1.0000 USD",
 *   "connectors": [ "EOSDT" ],
 *   "enabled": true,
 *   "metadata_json": {
 *     "key": "website", "value": "https://tether.io",
 *     "key": "description", "value": "Stable digital cash on the Blockchain"
 *   }
 * }
 * ```
 */
struct [[eosio::table("v1.pools"), eosio::contract("sx")]] v1_pools_row {
    extended_symbol         id;
    asset                   balance;
    asset                   depth;
    int64_t                 ratio;
    asset                   proceeds;
    int64_t                 amplifier = 1;
    symbol_code             type;
    asset                   pegged;
    set<symbol_code>        connectors;
    bool                    enabled = false;
    map<name, string>       metadata_json;

    uint64_t primary_key() const { return id.get_symbol().code().raw(); }
};
typedef eosio::multi_index< "v1.pools"_n, v1_pools_row > v1_pools_table;

/**
 * ## TABLE `volume`
 *
 * ### params
 *
 * - `{time_point_sec} timestamp` - daily periods
 * - `{map<symbol_code, asset} volume` - volume
 * - `{map<symbol_code, int64_t} volume` - count
 * - `{map<symbol_code, asset} depth` - depth
 * - `{map<symbol_code, asset} high_balance` - high_balance
 * - `{map<symbol_code, asset} low_balance` - low_balance
 * - `{map<symbol_code, int64_t} volatility` - volatility
 * - `{map<symbol_code, asset} proceeds` - proceeds
 * - `{map<symbol_code, int64_t} apr` - apr
 *
 * ### example
 *
 * ```json
 * {
 *   "timestamp": "2020-01-16T00:00:00"
 *   "volume": { "key": "USDT", "value": "5555 USDT" },
 *   "count": { "key": "USDT", "value": 10 },
 *   "depth": { "key": "USDT", "value": "10000 USDT" },
 *   "high_balance": { "key": "USDT", "value": "10100 USDT" },
 *   "low_balance": { "key": "USDT", "value": "9900 USDT" },
 *   "volatility": { "key": "USDT", "value": 9900 },
 *   "proceeds": { "key": "USDT", "value": "1.0000 USDT" },
 *   "apr": { "key": "USDT", "value": 224 }
 * }
 * ```
 */
struct [[eosio::table("volume"), eosio::contract("sx")]] volume_row {
    time_point_sec                  timestamp;

    // trading volume
    map<symbol_code, asset>         volume;
    map<symbol_code, int64_t>       count;

    // liquidity
    map<symbol_code, asset>         depth;
    map<symbol_code, asset>         high_balance;
    map<symbol_code, asset>         low_balance;
    map<symbol_code, int64_t>       volatility;

    // apr
    map<symbol_code, asset>         proceeds;
    map<symbol_code, int64_t>       apr;

    uint64_t primary_key() const { return timestamp.sec_since_epoch(); }
};
typedef eosio::multi_index< "volume"_n, volume_row > volume_table;

/**
 * ## TABLE `proceeds`
 *
 * - `{name} owner` - owner account name
 * - `{map<symbol_code, asset>} balances` - available balances
 *
 * ### example
 *
 * ```json
 * {
 *   "owner": "myaccount",
 *   "balances": [ { "key": "USDT", "value": "1.0000 USDT" } ]
 * }
 * ```
 */
struct [[eosio::table("proceeds"), eosio::contract("sx")]] proceeds_row {
    name                        owner;
    map<symbol_code, asset>     balances;

    uint64_t primary_key() const { return owner.value; }
};
typedef eosio::multi_index< "proceeds"_n, proceeds_row > proceeds_table;

/**
 * ## TABLE `admins`
 *
 * - `{name} account` - admin account name
 *
 * ### example
 *
 * ```json
 * {
 *   "account": "myaccount"
 * }
 * ```
 */
struct [[eosio::table("admins"), eosio::contract("sx")]] admins_row {
    name     account;

    uint64_t primary_key() const { return account.value; }
};
typedef eosio::multi_index< "admins"_n, admins_row > admins_table;

/**
 * ## TABLE `referrers`
 *
 * - `{name} referrer` - referrer account
 * - `{asset} transaction_fee` - transaction fee (ex: 0.01 USD)
 * - `{optional<string>} website` - referrer website
 * - `{optional<string>} description` - referrer description
 *
 * ### example
 *
 * ```json
 * {
 *   "referrer": "myreferrer",
 *   "transaction_fee": "0.01 USD",
 *   "metadata_json": [
 *     {"key": "website", "value": "https://mysite.com"},
 *     {"key": "description", "value": "My referrer UI"}
 *   ]
 * }
 * ```
 */
struct [[eosio::table("referrers"), eosio::contract("sx")]] referrers_row {
    name                        referrer;
    asset                       transaction_fee;
    map<eosio::name, string>    metadata_json;

    uint64_t primary_key() const { return referrer.value; }
};
typedef eosio::multi_index< "referrers"_n, referrers_row > referrers_table;

/**
 * ## TABLE `signups`
 *
 * - `{name} account` - signed up account
 * - `{name} referrer` - referrer account
 * - `{time_point_sec} timestamp` - timestamp of last `signup`
 *
 * ### example
 *
 * ```json
 * {
 *   "account": "myaccount",
 *   "referrer": "myreferrer",
 *   "timestamp": "2020-01-16T00:00:00"
 * }
 * ```
 */
struct [[eosio::table("signups"), eosio::contract("sx")]] signups_row {
    name                account;
    name                referrer;
    time_point_sec      timestamp;

    uint64_t primary_key() const { return account.value; }
};
typedef eosio::multi_index< "signups"_n, signups_row > signups_table;

/**
 * ## TABLE `account`
 *
 * - `{name} owner` - owner account name
 * - `{map<symbol_code, asset>} balances` - available balances
 *
 * ### example
 *
 * ```json
 * {
 *   "owner": "myaccount",
 *   "balances": [ { "key": "USDT", "value": "100.0000 USDT" } ]
 * }
 * ```
 */
struct [[eosio::table("accounts"), eosio::contract("sx")]] account {
    name                        owner;
    map<symbol_code, asset>     balances;

    uint64_t primary_key() const { return owner.value; }
};
typedef eosio::multi_index< "accounts"_n, account > accounts;

class [[eosio::contract("sx")]] sx : public contract {
    using contract::contract;

    /**
     * Construct a new contract given the contract name
     *
     * @param {name} receiver - The name of this contract
     * @param {name} code - The code name of the action this contract is processing.
     * @param {datastream} ds - The datastream used
     */
    sx( name receiver, name code, eosio::datastream<const char*> ds )
        : contract( receiver, code, ds )
    {}

    // @user
    [[eosio::action]]
    asset convert( const eosio::name owner, const eosio::asset quantity, const eosio::symbol_code symcode );

    // @user
    [[eosio::action]]
    asset autoconvert( const name owner, const asset quantity, const optional<asset> profit );

    // @user
    [[eosio::action]]
    void unstake( const name owner, const optional<asset> quantity );

    // @user
    [[eosio::action]]
    void claim( const name owner, const bool staked );

    // @user
    [[eosio::action]]
    void transfer( const name from, const name to, const asset quantity, const string memo );

    // @referrer
    [[eosio::action]]
    void signup( const name account, const optional<name> referrer );

    // @referrer
    [[eosio::action]]
    void setreferrer( const name referrer, const asset transaction_fee, const map<eosio::name, string> metadata_json );

    // @referrer
    [[eosio::action]]
    void delreferrer( const eosio::name referrer );

    // @contract
    [[eosio::action]]
    void receipt( const name owner, const name action, const list<asset> assets );

    static asset get_balance( const name contract, const name owner, const symbol_code symcode )
    {
        accounts _accounts( contract, contract.value );
        const auto account = _accounts.get( owner.value, "owner has no balance");
        const asset balance = account.balances.at( symcode );
        check( balance.symbol.code() == symcode, "no balance object found");
        return balance;
    }

    // @user
    using convert_action = eosio::action_wrapper<"convert"_n, &sx::convert>;
    using autoconvert_action = eosio::action_wrapper<"autoconvert"_n, &sx::autoconvert>;
    using unstake_action = eosio::action_wrapper<"unstake"_n, &sx::unstake>;
    using claim_action = eosio::action_wrapper<"claim"_n, &sx::claim>;

    // @referrer
    using setreferrer_action = eosio::action_wrapper<"setreferrer"_n, &sx::setreferrer>;
    using delreferrer_action = eosio::action_wrapper<"delreferrer"_n, &sx::delreferrer>;
    using signup_action = eosio::action_wrapper<"signup"_n, &sx::signup>;

    // @contract
    using receipt_action = eosio::action_wrapper<"receipt"_n, &sx::receipt>;
};
