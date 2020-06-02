#pragma once

#include <eosio/eosio.hpp>
#include <eosio/system.hpp>
#include <eosio/singleton.hpp>
#include <eosio/asset.hpp>

#include <math.h>
#include <string>

using namespace eosio;
using namespace std;

class [[eosio::contract]] stable : public contract {
public:
    using contract::contract;

    /**
     * ## TABLE `settings`
     *
     * - `{int64_t} fee` - trading fee (pips 1/100 of 1%)
     * - `{set<symbol_code>} spot_prices` - symbols for spot price calculations
     * - `{int64_t} amplifier` - liquidity pool amplifier
     * - `{extended_symbol} pool` - pool token ownership
     *
     * ### example
     *
     * ```json
     * {
     *   "fee": 10,
     *   "spot_prices": ["EOS"],
     *   "amplifier": 20,
     *   "pool": {
     *       "contract":"token.sx",
     *       "symbol": "4,SXS"
     *    }
     * }
     * ```
     */
    struct [[eosio::table("settings")]] params {
        int64_t             fee = 10;
        set<symbol_code>    spot_prices = set<symbol_code>{symbol_code{"USDT"}};
        int64_t             amplifier = 20;
        extended_symbol     pool;
    };
    typedef eosio::singleton< "settings"_n, params > settings;

    /**
     * ## TABLE `tokens`
     *
     * - `{symbol} sym` -  token symbol
     * - `{contract} contract` - token contract account name
     * - `{asset} balance` - current balance
     * - `{asset} depth` - liquidity depth
     * - `{uint64_t} [ratio=10000]` - ratio between balance & depth (pips 1/100 of 1%)
     * - `{bool} [active=false]` - (true/false) active token allowed to swap
     * - `{map<symbol_code, double>} spot_prices` - spot prices
     *
     * ### example
     *
     * ```json
     * {
     *     "sym": "4,USDT",
     *     "contract": "tethertether",
     *     "balance": "10000.0000 USDT",
     *     "depth": "10000.0000 USDT",
     *     "ratio": 10000,
     *     "active": true,
     *     "spot_prices": [
     *         {"key": "EOSDT", "value": 1.001}
     *     ]
     * }
     * ```
     */
    struct [[eosio::table("tokens")]] tokens_row {
        symbol                      sym;
        name                        contract;
        asset                       balance;
        asset                       depth;
        int64_t                     ratio;
        bool                        active;
        map<symbol_code, double>    spot_prices;

        uint64_t primary_key() const { return sym.code().raw(); }
    };
    typedef eosio::multi_index< "tokens"_n, tokens_row > tokens;

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
     * cleos push action swap.sx setparams '[{
     *     "fee": 10,
     *     "amplifier": 20,
     *     "pool": {"contract": "token.sx", "symbol": "4,SXS"}
     * }]' -p stable.sx
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
     * - `{bool} [active=false]` - (true/false) active token allowed to swap
     *
     * ### example
     *
     * ```bash
     * cleos push action swap.sx token '["USDT", "tethertether", true]' -p swap.sx
     * ```
     */
    [[eosio::action]]
    void token( const symbol_code symcode, const optional<name> contract, const bool active );

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

private:
    // utils
    symbol_code parse_memo_symcode( const string memo );
    double asset_to_double( const asset quantity );
    asset double_to_asset( const double amount, const symbol sym );

    // convert
    double calculate_out( const asset quantity, const asset base, const asset quote );
    asset calculate_fee( const asset quantity );

    // tokens
    void add_depth( const asset value );
    void sub_depth( const asset value );
    double get_ratio( const symbol_code symcode );
    void check_is_active( const symbol_code symcode, const name contract );
    void check_max_ratio( const asset quantity );
    void check_min_ratio( const asset out );
    name get_contract( const symbol_code symcode );
    symbol get_symbol( const symbol_code symcode );
    asset get_balance( const symbol_code symcode );
    extended_symbol get_extended_symbol( const symbol_code symcode );
};
