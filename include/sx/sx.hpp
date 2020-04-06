#pragma once

#include <eosio/eosio.hpp>
#include <eosio/system.hpp>
#include <eosio/singleton.hpp>
#include <eosio/asset.hpp>

#include <math.h>
#include <string>

using namespace eosio;
using namespace std;

// symbols
static constexpr symbol SYM_USD = symbol{"USD", 4};

// symbol codes
static constexpr symbol_code USD = symbol_code{"USD"};
static constexpr symbol_code EOS = symbol_code{"EOS"};
static constexpr symbol_code BTC = symbol_code{"BTC"};
static constexpr symbol_code EBTC = symbol_code{"EBTC"};
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
 * - `{int64_t} [pool_fee=6]` - trading fees towards liquidity pool providers (pips 1/100 of 1%)
 * - `{asset} [transaction_fee=0.00 USD]` - transaction fee paid to the contract
 * - `{int64_t} [stability_fee=0]` - stability fee towards contract (pips 1/100 of 1%)
 * - `{asset} [min_convert=1.00 USD]` - minimum amount to convert
 * - `{asset} [min_staked=1.00 USD]` - minimum amount to stake
 *
 * ### example
 *
 * ```json
 * {
 *   "paused": false,
 *   "transaction_fee": "0.0050 USD",
 *   "stability_fee": 0,
 *   "pool_fee": 10,
 *   "min_convert": "0.1000 USD",
 *   "min_staked": "10.0000 USD"
 * }
 * ```
 */
struct [[eosio::table("settings"), eosio::contract("sx")]] sx_parameters {
    bool            paused = false;
    int64_t         pool_fee = 10;
    asset           transaction_fee = asset{ 50, symbol{"USD", 4}};
    int64_t         stability_fee = 0;
    asset           min_convert = asset{ 1000, symbol{"USD", 4}};
    asset           min_staked = asset{ 100000, symbol{"USD", 4}};
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
 * - `{time_point_sec} timestamp` - daily periods (86400 seconds)
 * - `{map<symbol_code, asset} volume` - volume
 * - `{map<symbol_code, asset} proceeds` - proceeds
 *
 * ### example
 *
 * ```json
 * {
 *   "timestamp": "2020-01-16T00:00:00"
 *   "volume": { "key": "USDT", "value": "5555 USDT" },
 *   "proceeds": { "key": "USDT", "value": "1.0000 USDT" }
 * }
 * ```
 */
struct [[eosio::table("v1.volume"), eosio::contract("sx")]] v1_volume_row {
    time_point_sec                  timestamp;

    map<symbol_code, asset>         volume;
    map<symbol_code, asset>         proceeds;

    uint64_t primary_key() const { return timestamp.sec_since_epoch(); }
};
typedef eosio::multi_index< "v1.volume"_n, v1_volume_row > v1_volume_table;

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

private:
    // local instances of the multi indexes
    settings_table      _settings;
    proceeds_table      _proceeds;
    accounts            _accounts;
    v1_pools_table      _pools;
    v1_volume_table     _volume;

public:
    using contract::contract;

    /**
     * Construct a new contract given the contract name
     *
     * @param {name} receiver - The name of this contract
     * @param {name} code - The code name of the action this contract is processing.
     * @param {datastream} ds - The datastream used
     */
    sx( name receiver, name code, eosio::datastream<const char*> ds )
        : contract( receiver, code, ds ),
            _settings( get_self(), get_self().value ),
            _pools( get_self(), get_self().value ),
            _accounts( get_self(), get_self().value ),
            _proceeds( get_self(), get_self().value ),
            _volume( get_self(), get_self().value )
    {}

    // @user
    [[eosio::action]]
    void unstake( const name owner, const optional<asset> quantity );

    // @user
    [[eosio::action]]
    void claim( const name owner, const bool staked );

    // @contract
    [[eosio::action]]
    void receipt( const name owner, const name action, const list<asset> assets );

    // @admin
    [[eosio::action]]
    void init( );

    // @admin
    [[eosio::action]]
    void pause( const bool paused );

    // @admin
    [[eosio::action]]
    void create( const symbol_code symcode, const name contract, const symbol_code type );

    // @admin
    [[eosio::action]]
    void addconnector( const symbol_code symcode, const symbol_code connector );

    // @admin
    [[eosio::action]]
    void delconnector( const symbol_code symcode, const symbol_code connector );

    // @admin
    [[eosio::action]]
    void enable( const symbol_code symcode, const bool enabled );

    // @admin
    [[eosio::action]]
    void setmetadata( const symbol_code symcode, const map<name, string> metadata_json );

    // @admin
    [[eosio::action]]
    void setfee( const int64_t pool_fee, const optional<int64_t> stability_fee );

    // @admin
    [[eosio::action]]
    void setmin( const asset min_convert, const asset min_staked );

    // @admin
    [[eosio::action]]
    void setparams( const sx_parameters params );

    // @admin
    [[eosio::action]]
    void setamplifier( const symbol_code symcode, const int64_t amplifier );

    // @admin
    [[eosio::action]]
    void delpool( const symbol_code symcode );

    // @admin
    [[eosio::action]]
    void migrate( const name type );

    // @admin
    [[eosio::action]]
    void setpegged( const symbol_code symcode, const asset pegged );

    // @admin
    [[eosio::action]]
    void clean( const eosio::name table, const std::optional<eosio::name> scope );

    /**
     * Notify contract when any token transfer notifiers relay contract
     */
    [[eosio::on_notify("*::transfer")]]
    void on_transfer( const name       from,
                      const name       to,
                      const asset      quantity,
                      const string     memo );

    static asset get_balance( const name contract, const name owner, const symbol_code symcode )
    {
        accounts _accounts( contract, contract.value );
        const auto account = _accounts.get( owner.value, "owner has no balance");
        const asset balance = account.balances.at( symcode );
        check( balance.symbol.code() == symcode, "no balance object found");
        return balance;
    }

    // @user
    using unstake_action = eosio::action_wrapper<"unstake"_n, &sx::unstake>;
    using claim_action = eosio::action_wrapper<"claim"_n, &sx::claim>;

    // @admin
    using init_action = eosio::action_wrapper<"init"_n, &sx::init>;
    using pause_action = eosio::action_wrapper<"pause"_n, &sx::pause>;
    using create_action = eosio::action_wrapper<"create"_n, &sx::create>;
    using addconnector_action = eosio::action_wrapper<"addconnector"_n, &sx::addconnector>;
    using delconnector_action = eosio::action_wrapper<"delconnector"_n, &sx::delconnector>;
    using delpool_action = eosio::action_wrapper<"delpool"_n, &sx::delpool>;
    using enable_action = eosio::action_wrapper<"enable"_n, &sx::enable>;
    using setmetadata_action = eosio::action_wrapper<"setmetadata"_n, &sx::setmetadata>;
    using setfee_action = eosio::action_wrapper<"setfee"_n, &sx::setfee>;
    using setmin_action = eosio::action_wrapper<"setmin"_n, &sx::setmin>;
    using setparams_action = eosio::action_wrapper<"setparams"_n, &sx::setparams>;
    using setamplifier_action = eosio::action_wrapper<"setamplifier"_n, &sx::setamplifier>;

    // @contract
    using receipt_action = eosio::action_wrapper<"receipt"_n, &sx::receipt>;

private:
    // utils
    symbol_code parse_memo_symcode( const vector<string> tokens );

    double asset_to_double( const asset quantity );
    asset double_to_asset( const double amount, const symbol sym );
    vector<string> split( const string str, const string delim );

    // proceeds
    void add_proceeds_owner( const name owner, const asset value );
    void sub_proceeds_owner( const name owner, const asset value );

    // accounts
    void add_balance( const name owner, const asset value );
    void sub_balance( const name owner, const asset value );
    void clear_empty_balances( const name owner );
    void unstake_all( const name owner );
    void check_available_balance( const asset quantity );
    void transfer_out( const name to, const asset quantity, const string memo );

    // convert
    asset calculate_out( const asset quantity, const symbol_code symcode );
    asset calculate_pool_fee( const asset quantity );
    name get_contract( const symbol_code symcode );
    void check_pool_thresholds( const asset in, const asset out );
    void check_min_convert( const asset quantity );
    double get_ratio( const symbol_code symcode );
    asset convert( const name owner, const asset quantity, const symbol_code symcode );
    asset convert_out( const name owner, const asset quantity, const symbol_code symcode );
    void check_max_pool_ratio( const asset quantity );
    void check_min_pool_ratio( const asset out );

    // pool
    void add_depth( const asset value );
    void sub_depth( const asset value );
    void add_proceeds( const asset value );
    void sub_proceeds( const asset value );
    void update_pool_ratio( const symbol_code symcode );
    void update_pool_ratios();
    void check_pool_contract( const symbol_code symcode, const name contract );
    void update_pool_depths();
    bool pool_exists( const symbol_code symcode );

    // staked
    void add_staked( const name owner, const asset value );
    void sub_staked( const name owner, const asset value );
    void check_min_staked( const name owner, const symbol_code symcode );

    // claim
    void claim_pool( const symbol_code symcode );
    void claim_pools();
    list<asset> claim_staked( const name owner );
    list<asset> claim_transfer_out( const name owner );

    // volume
    void add_volume( const asset volume, const asset proceeds );
    void update_total_volume( const time_point_sec timestamp );

    // receipt
    void send_receipt( const name owner, const name action, const list<asset> assets );

    // admins
    void require_auth_or_self( name owner );

    // prices
    asset get_pegged( const symbol_code symcode );
};
