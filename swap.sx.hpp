#pragma once

#include <eosio/eosio.hpp>
#include <eosio/system.hpp>
#include <eosio/singleton.hpp>
#include <eosio/asset.hpp>

#include <sx.uniswap/uniswap.hpp>

#include <math.h>
#include <string>

using namespace eosio;
using namespace std;

namespace sx {
class [[eosio::contract("swap.sx")]] swap : public contract {
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
    void setparams( const optional<sx::swap::params> params );

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
     * const asset amount_out = sx::swap::get_amount_out( contract, amount_in, symcode_out );
     * // => "2.7328 USDT"
     * ```
     */
    static asset get_amount_out( const name contract, const asset amount_in, const symbol_code symcode_out )
    {
        sx::swap::settings _settings( contract, contract.value );
        check( _settings.exists(), contract.to_string() + " settings are not initialized");

        // settings
        const int64_t fee = _settings.get().fee;
        const symbol_code symcode_in = amount_in.symbol.code();

        // reserves
        const auto [ virtual_reserve_in, virtual_reserve_out ] = sx::swap::get_virtual_reserves( contract, symcode_in, symcode_out );
        const auto [ reserve_in, reserve_out ] = sx::swap::get_reserves( contract, symcode_in, symcode_out );

        // calculate out
        const int64_t out = uniswap::get_amount_out( amount_in.amount, virtual_reserve_in.amount, virtual_reserve_out.amount, fee );
        if ( reserve_out.amount < out ) return { 0, reserve_out.symbol };
        return { out, reserve_out.symbol };
    }

    /**
     * ## STATIC `get_virtual_reserves`
     *
     * Get virtual reserves for a pair
     *
     * ### params
     *
     * - `{symbol_code} symcode_in` - incoming symbol code
     * - `{symbol_code} symcode_out` - outgoing symbol code
     *
     * ### returns
     *
     * - `{pair<asset, asset>}` - pair of reserve assets
     *
     * ### example
     *
     * ```c++
     * const name contract = "swap.sx"_n;
     * const symbol_code symcode_in = symbol_code{"EOS"};
     * const symbol_code symcode_out = symbol_code{"USDT"};
     *
     * const auto [reserve0, reserve1] = sx::swap::get_virtual_reserves( contract, symcode_in, symcode_out );
     * // reserve0 => "50000.0000 EOS"
     * // reserve1 => "150000.0000 USDT"
     * ```
     */
    static std::pair<asset, asset> get_virtual_reserves(const name contract, const symbol_code symcode_in, const symbol_code symcode_out )
    {
        // table
        sx::swap::tokens _tokens( contract, contract.value );

        const asset reserve_in = _tokens.get( symcode_in.raw(), "[symcode_in] token does not exists").virtual_reserve;
        const asset reserve_out = _tokens.get( symcode_out.raw(), "[symcode_out] token does not exists").virtual_reserve;

        return std::pair<asset, asset>{ reserve_in, reserve_out };
    }

    /**
     * ## STATIC `get_reserves`
     *
     * Get reserves for a pair
     *
     * ### params
     *
     * - `{symbol_code} symcode_in` - incoming symbol code
     * - `{symbol_code} symcode_out` - outgoing symbol code
     *
     * ### returns
     *
     * - `{pair<asset, asset>}` - pair of reserve assets
     *
     * ### example
     *
     * ```c++
     * const name contract = "swap.sx"_n;
     * const symbol_code symcode_in = symbol_code{"EOS"};
     * const symbol_code symcode_out = symbol_code{"USDT"};
     *
     * const auto [reserve0, reserve1] = sx::swap::get_reserves( contract, symcode_in, symcode_out );
     * // reserve0 => "250.0000 EOS"
     * // reserve1 => "750.0000 USDT"
     * ```
     */
    static std::pair<asset, asset> get_reserves(const name contract, const symbol_code symcode_in, const symbol_code symcode_out )
    {
        // table
        sx::swap::tokens _tokens( contract, contract.value );

        const asset reserve_in = _tokens.get( symcode_in.raw(), "[symcode_in] token does not exists").reserve;
        const asset reserve_out = _tokens.get( symcode_out.raw(), "[symcode_out] token does not exists").reserve;

        return std::pair<asset, asset>{ reserve_in, reserve_out };
    }

    /**
     * ## STATIC `is_available`
     *
     * Calculate if contract has enough reserve to cover out quantity.
     *
     * ### params
     *
     * - `{name} contract` - swap contract
     * - `{asset} out` - out quantity
     *
     * ### returns
     *
     * - `{bool}` - (true/false) if reserve has enough to cover quantity
     *
     * ### example
     *
     * ```c++
     * const name contract = "swap.sx"_n;
     * const asset out = asset{10000, symbol{"EOS", 4}};
     *
     * const bool available = sx::swap::is_available( contract, out );
     * // => true/false
     * ```
     */
    static bool is_available( const name contract, const asset out )
    {
        // table
        sx::swap::tokens _tokens( contract, contract.value );
        const asset reserve = _tokens.get( out.symbol.code().raw(), "[out] token does not exists").reserve;
        return out >= reserve;
    }

    // action wrappers
    using setparams_action = eosio::action_wrapper<"setparams"_n, &sx::swap::setparams>;
    using token_action = eosio::action_wrapper<"token"_n, &sx::swap::token>;

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
}