#pragma once

#include <eosio/eosio.hpp>
#include <eosio/system.hpp>
#include <eosio/singleton.hpp>
#include <eosio/asset.hpp>

#include <math.h>
#include <string>

using namespace eosio;
using namespace std;

class [[eosio::contract("stable.sx")]] stable : public contract {
public:
    using contract::contract;

    /**
     * ## TABLE `settings`
     *
     * - `{int64_t} fee` - trading fee (pips 1/100 of 1%)
     * - `{int64_t} amplifier` - liquidity pool amplifier
     *
     * ### example
     *
     * ```json
     * {
     *   "fee": 20,
     *   "amplifier": 100
     * }
     * ```
     */
    struct [[eosio::table("settings")]] params {
        int64_t             fee;
        int64_t             amplifier;
    };
    typedef eosio::singleton< "settings"_n, params > settings;

    /**
     * ## TABLE `tokens`
     *
     * - `{symbol} sym` -  token symbol
     * - `{contract} contract` - token contract account name
     * - `{asset} balance` - current balance
     * - `{asset} depth` - liquidity depth
     *
     * ### example
     *
     * ```json
     * {
     *     "sym": "4,USDT",
     *     "contract": "tethertether",
     *     "balance": "10000.0000 USDT",
     *     "depth": "10000.0000 USDT"
     * }
     * ```
     */
    struct [[eosio::table("tokens")]] tokens_row {
        symbol                      sym;
        name                        contract;
        asset                       balance;
        asset                       depth;

        uint64_t primary_key() const { return sym.code().raw(); }
    };
    typedef eosio::multi_index< "tokens"_n, tokens_row > tokens;

    /**
     * ## TABLE `volume`
     *
     * - `{time_point_sec} timestamp` - daily periods (86400 seconds)
     * - `{map<symbol_code, asset>} fees` - total fees collected
     * - `{map<symbol_code, asset>} volume` - total trading volume of assets
     *
     * ### example
     *
     * ```json
     * {
     *   "timestamp": "2020-06-03T00:00:00",
     *   "volume": [
     *     {"key": "EOSDT", "value": "25.000000000 EOSDT"},
     *     {"key": "USDT", "value": "100.0000 USDT"}
     *   ],
     *   "fees": [
     *     {"key": "EOSDT", "value": "0.100000000 EOSDT"},
     *     {"key": "USDT", "value": "0.4000 USDT"}
     *   ]
     * }
     * ```
     */
    struct [[eosio::table("volume")]] volume_params {
        time_point_sec             timestamp;
        map<symbol_code, asset>    volume;
        map<symbol_code, asset>    fees;
    };
    typedef eosio::singleton< "volume"_n, volume_params > volume_singleton;

    /**
     * ## ACTION `setparams`
     *
     * Update contract parameters
     *
     * - **authority**: `get_self()`
     *
     * ### params
     *
     * - `{settings_params} settings` - settings parameters
     *
     * ### example
     *
     * ```bash
     * cleos push action swap.sx setparams '[{"fee": 10, "amplifier": 20}]' -p stable.sx
     * ```
     */
    [[eosio::action]]
    void setparams( const optional<stable::params> params );

    /**
     * ## ACTION `token`
     *
     * Add/removes token
     *
     * - **authority**: `get_self()`
     *
     * ### params
     *
     * - `{symbol_code} symcode` - token symbol code
     * - `{name} [contract=null]` - token contract account name (if `null` delete symbol)
     *
     * ### example
     *
     * ```bash
     * cleos push action swap.sx token '["USDT", "tethertether"]' -p swap.sx
     * ```
     */
    [[eosio::action]]
    void token( const symbol_code symcode, const optional<name> contract );

    /**
     * ## ACTION `receipt`
     *
     * Receipt of transaction
     *
     * - **authority**: `get_self()`
     *
     * ### params
     *
     * - `{name} owner` - owner account
     * - `{name} action` - receipt action ("convert")
     * - `{list<asset>} assets` - assets involved in receipt
     *
     * ### example
     *
     * ```bash
     * cleos push action stable.sx receipt '["myaccount", "convert", ["1.0000 EOS", "2.7200 USDT"]]' -p stable.sx
     * ```
     */
    [[eosio::action]]
    void receipt( const name owner, const name action, const list<asset> assets );

    /**
     * Notify contract when any token transfer notifiers relay contract
     */
    [[eosio::on_notify("*::transfer")]]
    void on_transfer( const name       from,
                      const name       to,
                      const asset      quantity,
                      const string     memo );

    // action wrappers
    using setparams_action = eosio::action_wrapper<"setparams"_n, &stable::setparams>;
    using token_action = eosio::action_wrapper<"token"_n, &stable::token>;
    using receipt_action = eosio::action_wrapper<"receipt"_n, &stable::receipt>;

private:
    // utils
    symbol_code parse_memo_symcode( const string memo );
    double asset_to_double( const asset quantity );
    asset double_to_asset( const double amount, const symbol sym );
    void self_transfer( const name to, const asset quantity, const string memo );

    // convert
    double calculate_out( const asset quantity, const symbol_code out_symcode );
    asset calculate_fee( const asset quantity );

    // tokens
    void set_balance( const symbol_code symcode );
    void add_depth( const asset quantity );
    void sub_depth( const asset quantity );

    void check_is_active( const symbol_code symcode, const name contract );
    void check_max_ratio( const symbol_code symcode );
    void check_min_ratio( const asset out );

    double get_ratio( const symbol_code symcode );
    name get_contract( const symbol_code symcode );
    symbol get_symbol( const symbol_code symcode );
    asset get_balance( const symbol_code symcode );
    extended_symbol get_extended_symbol( const symbol_code symcode );
    asset get_depth( const symbol_code symcode );

    // volume
    void update_volume( const vector<asset> volumes, const asset fee );

    // receipt
    void send_receipt( const name owner, const name action, const list<asset> assets );
};
