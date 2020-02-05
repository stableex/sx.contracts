#include <eosio/eosio.hpp>
#include <eosio/system.hpp>
#include <eosio/singleton.hpp>
#include <eosio/asset.hpp>

#include <math.h>
#include <string>

using namespace eosio;
using namespace std;

/**
 * ## TABLE `settings`
 *
 * - `{int64_t} [min_pool_threshold=2000]` - minimum pool threshold (balance %)
 * - `{bool} [paused=false]` - (true/false) entire contract paused
 * - `{asset} [transaction_fee=0.00 USD]` - transaction fee paid to the contract
 * - `{int64_t} [fee=6]` - trading fee (pips 1/100 of 1%)
 * - `{asset} [min_convert=1.00 USD]` - minimum amount to convert
 * - `{asset} [min_staked=1.00 USD]` - minimum amount to stake
 * - `{int64_t} [multiplier=20]` - liquidity multiplier (ex: x20)
 *
 * ### example
 *
 * ```json
 * {
 *   "min_pool_threshold": 2000,
 *   "paused": false,
 *   "transaction_fee": "0.00 USD",
 *   "fee": 6,
 *   "min_convert": "1.00 USD",
 *   "min_staked": "1.00 USD",
 *   "multiplier": 20
 * }
 * ```
 */
struct [[eosio::table("settings"), eosio::contract("stable")]] stable_parameters {
    int64_t         min_pool_threshold = 2000;
    bool            paused = false;
    asset           transaction_fee = asset{0'00, symbol{"USD", 4}};
    int64_t         fee = 6;
    asset           min_convert = asset{1'00, symbol{"USD", 4}};
    asset           min_staked = asset{1'00, symbol{"USD", 4}};
    int64_t         multiplier = 20;
};
typedef eosio::singleton< "settings"_n, stable_parameters> settings_table;

/**
 * ## TABLE `pools`
 *
 * - `{extended_symbol} id` - extended symbol ID
 * - `{asset} [balance=0.0000 USD]` - remaining balance
 * - `{asset} [depth=0.0000 USD]` - liquidity depth
 * - `{uint64_t} [ratio=10000]` - ratio between balance & depth
 * - `{asset} [proceeds=0.0000 USD]` - accumulated trading fee proceeds
 * - `{symbol_code} [type=USD]` - liquidity type (ex: USD, EUR, CNY)
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
 *   "type": "USD",
 *   "pegged": 10000,
 *   "connectors": [ "EOSDT" ],
 *   "enabled": true,
 *   "metadata_json": {
 *     "key": "website", "value": "https://tether.io",
 *     "key": "description", "value": "Stable digital cash on the Blockchain"
 *   }
 * }
 * ```
 */
struct [[eosio::table("pools"), eosio::contract("stable")]] pools_row {
    extended_symbol         id;
    asset                   balance;
    asset                   depth;
    int64_t                 ratio;
    asset                   proceeds;
    int64_t                 pegged;
    symbol_code             type;
    set<symbol_code>        connectors;
    bool                    enabled = false;
    map<name, string>       metadata_json;

    uint64_t primary_key() const { return id.get_symbol().code().raw(); }
};
typedef eosio::multi_index< "pools"_n, pools_row > pools_table;

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
struct [[eosio::table("volume"), eosio::contract("stable")]] volume_row {
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
struct [[eosio::table("accounts"), eosio::contract("stable")]] account {
    name                        owner;
    map<symbol_code, asset>     balances;

    uint64_t primary_key() const { return owner.value; }
};
typedef eosio::multi_index< "accounts"_n, account > accounts;

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
struct [[eosio::table("proceeds"), eosio::contract("stable")]] proceeds_row {
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
struct [[eosio::table("admins"), eosio::contract("stable")]] admins_row {
    name     account;

    uint64_t primary_key() const { return account.value; }
};
typedef eosio::multi_index< "admins"_n, admins_row > admins_table;

/**
 * ## TABLE `referrals`
 *
 * - `{name} name` - referral account
 * - `{asset} transaction_fee` - transaction fee (ex: 0.01 USD)
 * - `{optional<string>} website` - referral website
 * - `{optional<string>} description` - referral description
 *
 * ### example
 *
 * ```json
 * {
 *   "name": "myreferral",
 *   "transaction_fee": "0.01 USD",
 *   "metadata_json": [
 *     {"key": "website", "value": "https://referral.com"},
 *     {"key": "description", "value": "My referral UI"}
 *   ]
 * }
 * ```
 */
struct [[eosio::table("referrals"), eosio::contract("stable")]] referrals_row {
    name                        name;
    asset                       transaction_fee;
    map<eosio::name, string>    metadata_json;

    uint64_t primary_key() const { return name.value; }
};
typedef eosio::multi_index< "referrals"_n, referrals_row > referrals_table;

/**
 * ## TABLE `signups`
 *
 * - `{name} account` - signed up account
 * - `{name} referral` - referral account
 * - `{time_point_sec} timestamp` - timestamp of last `signup`
 *
 * ### example
 *
 * ```json
 * {
 *   "account": "myaccount",
 *   "referral": "myreferral",
 *   "timestamp": "2020-01-16T00:00:00"
 * }
 * ```
 */
struct [[eosio::table("signups"), eosio::contract("stable")]] signups_row {
    name                account;
    name                referral;
    time_point_sec      timestamp;

    uint64_t primary_key() const { return account.value; }
};
typedef eosio::multi_index< "signups"_n, signups_row > signups_table;

class [[eosio::contract("stable")]] stable : public contract {
public:
    using contract::contract;

    /**
     * Construct a new contract given the contract name
     *
     * @param {name} receiver - The name of this contract
     * @param {name} code - The code name of the action this contract is processing.
     * @param {datastream} ds - The datastream used
     */
    stable( name receiver, name code, eosio::datastream<const char*> ds )
        : contract( receiver, code, ds ),
            _settings( get_self(), get_self().value ),
            _pools( get_self(), get_self().value ),
            _accounts( get_self(), get_self().value ),
            _proceeds( get_self(), get_self().value ),
            _referrals( get_self(), get_self().value ),
            _admins( get_self(), get_self().value ),
            _volume( get_self(), get_self().value ),
            _signups( get_self(), get_self().value )
    {}

    // @user
    [[eosio::action]]
    asset convert( const eosio::name owner, const eosio::asset quantity, const eosio::symbol_code symcode );

    // @user
    [[eosio::action]]
    asset autoconvert( const name owner, const asset quantity );

    // @user
    [[eosio::action]]
    void unstake( const name owner, const asset quantity );

    // @user
    [[eosio::action]]
    void claim( const name owner, const bool staked );

    // @user
    [[eosio::action]]
    void close( const name account );

    // @referral
    [[eosio::action]]
    void signup( const name account, const optional<name> referral );

    // @referral
    [[eosio::action]]
    void setreferral( const name name, const asset transaction_fee, const map<eosio::name, string> metadata_json );

    // @referral
    [[eosio::action]]
    void delreferral( const eosio::name name );

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
    void multiplier( const int64_t multiplier );

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
    void setfee( const int64_t fee, const asset transaction_fee );

    // @admin
    [[eosio::action]]
    void setmin( const asset min_convert, const asset min_staked );

    // @admin
    [[eosio::action]]
    void setparams( const stable_parameters params );

    // @admin
    void setminpool( const int64_t min_pool_threshold );

    // @admin
    [[eosio::action]]
    void setadmin( const name account, const bool enabled );

    // @admin
    [[eosio::action]]
    void setpegged( const symbol_code symcode, const int64_t pegged );

    // @admin
    [[eosio::action]]
    void balancepools();

    // @admin
    [[eosio::action]]
    void delpool( const symbol_code symcode );

    // @admin
    [[eosio::action]]
    void migrate( const name type );

    /**
     * Notify contract when any token transfer notifiers relay contract
     */
    [[eosio::on_notify("*::transfer")]]
    void transfer( const eosio::name    from,
                   const eosio::name    to,
                   const eosio::asset   quantity,
                   const eosio::string  memo );

    static asset get_balance( const name contract, const name owner, const symbol_code symcode )
    {
        accounts _accounts( contract, contract.value );
        const auto account = _accounts.get( owner.value, "owner has no balance");
        const asset balance = account.balances.at( symcode );
        check( balance.symbol.code() == symcode, "no balance object found");
        return balance;
    }

    // @user
    using convert_action = eosio::action_wrapper<"convert"_n, &stable::convert>;
    using autoconvert_action = eosio::action_wrapper<"autoconvert"_n, &stable::autoconvert>;
    using unstake_action = eosio::action_wrapper<"unstake"_n, &stable::unstake>;
    using claim_action = eosio::action_wrapper<"claim"_n, &stable::claim>;
    using close_action = eosio::action_wrapper<"close"_n, &stable::close>;

    // @referral
    using setreferral_action = eosio::action_wrapper<"setreferral"_n, &stable::setreferral>;
    using delreferral_action = eosio::action_wrapper<"delreferral"_n, &stable::delreferral>;
    using signup_action = eosio::action_wrapper<"signup"_n, &stable::signup>;

    // @admin
    using init_action = eosio::action_wrapper<"init"_n, &stable::init>;
    using pause_action = eosio::action_wrapper<"pause"_n, &stable::pause>;
    using create_action = eosio::action_wrapper<"create"_n, &stable::create>;
    using multiplier_action = eosio::action_wrapper<"multiplier"_n, &stable::multiplier>;
    using addconnector_action = eosio::action_wrapper<"addconnector"_n, &stable::addconnector>;
    using delconnector_action = eosio::action_wrapper<"delconnector"_n, &stable::delconnector>;
    using delpool_action = eosio::action_wrapper<"delpool"_n, &stable::delpool>;
    using enable_action = eosio::action_wrapper<"enable"_n, &stable::enable>;
    using setmetadata_action = eosio::action_wrapper<"setmetadata"_n, &stable::setmetadata>;
    using balancepools_action = eosio::action_wrapper<"balancepools"_n, &stable::balancepools>;
    using setfee_action = eosio::action_wrapper<"setfee"_n, &stable::setfee>;
    using setmin_action = eosio::action_wrapper<"setmin"_n, &stable::setmin>;
    using setparams_action = eosio::action_wrapper<"setparams"_n, &stable::setparams>;

    // @contract
    using receipt_action = eosio::action_wrapper<"receipt"_n, &stable::receipt>;

private:
    // local instances of the multi indexes
    settings_table      _settings;
    pools_table         _pools;
    volume_table        _volume;
    accounts            _accounts;
    proceeds_table      _proceeds;
    admins_table        _admins;
    referrals_table     _referrals;
    signups_table       _signups;

    // on_notify
    void on_eos_transfer( const name from, const name to, const asset quantity, const string memo );

    // utils
    symbol_code parse_memo_symcode( const vector<string> tokens );
    name parse_memo_referral( const vector<string> tokens );

    double asset_to_double( const asset quantity );
    asset double_to_asset( const double amount, const symbol sym );
    vector<eosio::string> split( const string str, const string delim );

    // proceeds
    void transfer_proceeds_out( const name to, const asset quantity, const string memo );
    void add_proceeds_owner( const name owner, const asset value );
    void sub_proceeds_owner( const name owner, const asset value );

    // accounts
    void transfer_out( const name to, const asset quantity, const string memo );
    void add_balance( const name owner, const asset value );
    void sub_balance( const name owner, const asset value );

    // convert
    asset calculate_out( const asset quantity, const symbol_code symcode );
    asset calculate_pool_fee( const asset quantity );
    name get_contract( const symbol_code symcode );
    void check_pool_min_threshold( const asset out );
    void check_min_convert( const asset quantity );
    double get_ratio( const symbol_code symcode );
    asset convert_out( const name owner, const asset quantity, const symbol_code symcode );
    asset calculate_transaction_fee( const symbol sym );

    // pool
    void add_depth( const asset value );
    void sub_depth( const asset value );
    void add_proceeds( const asset value );
    void sub_proceeds( const asset value );
    void update_pool_ratio( const symbol_code symcode );
    void update_pool_ratios();
    void check_pool_contract( const symbol_code symcode, const name contract );

    // staked
    void add_staked( const name owner, const asset value );
    void sub_staked( const name owner, const asset value );
    void check_min_staked( const name owner, const symbol_code symcode );

    // claim
    void claim_pool( const symbol_code symcode );
    void claim_transfer_out( const name owner );
    void claim_staked( const name owner );

    // volume
    void add_volume( const asset quantity, const asset proceeds );
    void update_volume_depth( const symbol_code symcode );
    int64_t calculate_apr( const asset proceeds, const asset depth );
    int64_t calculate_volatility( const asset low_balance, const asset high_balance );

    // receipt
    void send_receipt( const name owner, const name action, const list<asset> assets );

    // admins
    void require_auth_or_admin( name owner );

    // referrals
    name get_referral( const name account );
    asset get_transaction_fee( const name referral, const symbol sym );
};
