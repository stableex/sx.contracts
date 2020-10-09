#pragma once

#include <eosio/eosio.hpp>
#include <eosio/system.hpp>
#include <eosio/singleton.hpp>
#include <eosio/asset.hpp>

#include <math.h>
#include <string>

using namespace eosio;
using namespace std;

class [[eosio::contract("swap.sx")]] swapSx : public contract {
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
     *   "amplifier": 20
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
     * - `{asset} reserve` - liquidity reserve
     * - `{asset} virtual_reserve` - virtual liquidity reserve
     *
     * ### example
     *
     * ```json
     * {
     *     "sym": "4,USDT",
     *     "contract": "tethertether",
     *     "balance": "10000.0000 USDT",
     *     "depth": "10000.0000 USDT",
     *     "reserve": "10000.0000 USDT",
     *     "virtual_reserve": "200000.0000 USDT"
     * }
     * ```
     */
    struct [[eosio::table("tokens")]] tokens_row {
        symbol                      sym;
        name                        contract;
        asset                       balance;
        asset                       depth;
        asset                       reserve;
        asset                       virtual_reserve;

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
     * cleos push action swap.sx setparams '[{"fee": 50, "amplifier": 20, "base": "EOS"}]' -p swap.sx
     * ```
     */
    [[eosio::action]]
    void setparams( const optional<swapSx::params> params );

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
     * ## ACTION `swaplog`
     *
     * Notify of trade
     *
     * - **authority**: `get_self()`
     *
     * ### params
     *
     * - `{name} buyer` - trader buyer account
     * - `{asset} amount_in` - amount incoming
     * - `{asset} amount_out` - amount outgoing
     * - `{asset} fee` - fee paid
     *
     * ### example
     *
     * ```bash
     * cleos push action swap.sx swaplog '["myaccount", "3.0000 EOS", "7.0486 USDT", "0.0060 EOS"]' -p swap.sx
     * ```
     */
    [[eosio::action]]
    void swaplog( const name buyer,
                  const asset amount_in,
                  const asset amount_out,
                  const asset fee );

    /**
     * Notify contract when any token transfer notifiers relay contract
     */
    [[eosio::on_notify("*::transfer")]]
    void on_transfer( const name       from,
                      const name       to,
                      const asset      quantity,
                      const string     memo );

    [[eosio::action]]
    void clear();

    /**
     * ## STATIC `get_amount_out`
     *
     * Given an input amount of an asset and pair reserves, returns the maximum output amount of the other asset
     *
     * ### params
     *
     * - `{name} contract` - contract name
     * - `{asset} amount_in` - amount input
     * - `{asset} symcode_out` - symbol code out
     *
     * ### example
     *
     * ```c++
     * // Inputs
     * const name contract = "swap.sx"_n;
     * const asset amount_in = asset{10000, symbol{"EOS", 4}};
     * const asset symcode_out = symbol_code{"USDT"};
     *
     * // Calculation
     * const asset amount_out = swapSx::get_amount_out( contract, amount_in, symcode_out );
     * // => "2.7328 USDT"
     * ```
     */
    static asset get_amount_out( const name contract, const asset amount_in, const symbol_code symcode_out )
    {
        swapSx::settings _settings( contract, contract.value );
        swapSx::tokens _tokens( contract, contract.value );

        check(_settings.exists(), contract.to_string() + " settings are not initialized");

        const int64_t fee = _settings.get().fee;
        const asset reserve_in = _tokens.get( amount_in.symbol.code().raw(), "[amount_in] token does not exists").virtual_reserve;
        const asset reserve_out = _tokens.get( symcode_out.raw(), "[symbol_out] token does not exists").virtual_reserve;

        return uniswap::get_amount_out( amount_in, reserve_in, reserve_out, fee );
    }

    // action wrappers
    using setparams_action = eosio::action_wrapper<"setparams"_n, &swapSx::setparams>;
    using token_action = eosio::action_wrapper<"token"_n, &swapSx::token>;
    using swaplog_action = eosio::action_wrapper<"swaplog"_n, &swapSx::swaplog>;

private:
    // utils
    symbol_code parse_memo_symcode( const string memo );
    void self_transfer( const name to, const asset quantity, const string memo );
    name get_contract( const name code, const symbol_code symcode );
    symbol get_symbol( const name code, const symbol_code symcode );
    extended_symbol get_extended_symbol( const name code, const symbol_code symcode );

    // virtual reserve
    asset get_upper( const symbol_code symcode );

    // tokens
    void add_balance( const asset quantity );
    void sub_balance( const asset quantity );
    void set_reserve( const symbol_code symcode );
    void set_virtual_reserve( const symbol_code symcode );
    bool is_token_exists( const symbol_code symcode );

    void check_is_active( const symbol_code symcode, const name contract );
    void check_remaining_balance( const asset out );

    // settings
    void erase_all_tokens();

    template <typename T>
    void clear_table( T& table );
};
