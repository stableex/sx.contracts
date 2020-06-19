#pragma once

#include <eosio/eosio.hpp>
#include <eosio/system.hpp>
#include <eosio/singleton.hpp>
#include <eosio/asset.hpp>

#include <math.h>
#include <string>

using namespace eosio;
using namespace std;

// settings
static constexpr symbol_code SPOT_PRICE_BASE = symbol_code{"USDT"};

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
     *   "fee": 50,
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
     * ## TABLE `docs`
     *
     * - `{string} url` - Documentation url
     *
     * ### example
     *
     * ```json
     * {
     *   "url": "https://github.com/stableex/sx.swap"
     * }
     * ```
     */
    struct [[eosio::table("docs")]] docs_row {
        string      url = "https://github.com/stableex/sx.swap";
    };
    typedef eosio::singleton< "docs"_n, docs_row > docs;

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
    typedef eosio::multi_index< "tokens"_n, tokens_row > tokens_table;

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
     *     "timestamp": "2020-06-03T00:00:00",
     *     "volume": [
     *         {"key": "EOS", "value": "25.0000 EOS"},
     *         {"key": "USDT", "value": "100.0000 USDT"}
     *     ],
     *     "fees": [
     *         {"key": "EOS", "value": "0.1250 EOS"},
     *         {"key": "USDT", "value": "0.5000 USDT"}
     *     ]
     * }
     * ```
     */
    struct [[eosio::table("volume")]] volume_params {
        time_point_sec             timestamp;
        map<symbol_code, asset>    volume;
        map<symbol_code, asset>    fees;
    };
    typedef eosio::singleton< "volume"_n, volume_params > volume_table;

    /**
     * ## TABLE `spotprices`
     *
     * - `{time_point_sec} last_modified` - last modified timestamp
     * - `{symbol_code} fees` - base symbol code
     * - `{map<symbol_code, double>} quotes` - quotes prices calculated relative to base
     *
     * ### example
     *
     * ```json
     * {
     *   "last_modified": "2020-06-03T12:30:00",
     *   "base": "EOS",
     *   "quotes": [
     *     {"key": "EOSDT", "value": 0.3636},
     *     {"key": "USDT", "value": 0.3636}
     *   ]
     * }
     * ```
     */
    struct [[eosio::table("spotprices")]] spotprices_params {
        time_point_sec              last_modified;
        symbol_code                 base;
        map<symbol_code, double>    quotes;
    };
    typedef eosio::singleton< "spotprices"_n, spotprices_params > spotprices_table;

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
     * cleos push action swap.sx setparams '[{"fee": 50, "amplifier": 20}]' -p swap.sx
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
     * ## ACTION `log`
     *
     * Notify of trade
     *
     * - **authority**: `get_self()`
     *
     * ### params
     *
     * - `{name} buyer` - trader buyer account
     * - `{asset} quantity` - incoming quantity
     * - `{asset} rate` - outgoing rate
     * - `{asset} fee` - fee paid per trade
     * - `{double} trade_price` - trade price per unit
     * - `{double} spot_price` - spot price per rate
     *
     * ### example
     *
     * ```bash
     * cleos push action swap.sx log '["myaccount", "1.0000 EOS", "2.5300 USDT", "0.0050 EOS", 2.53]' -p swap.sx
     * ```
     */
    [[eosio::action]]
    void log( const name buyer,
              const asset quantity,
              const asset rate,
              const asset fee,
              const double trade_price,
              const double spot_price );

    /**
     * Notify contract when any token transfer notifiers relay contract
     */
    [[eosio::on_notify("*::transfer")]]
    void on_transfer( const name       from,
                      const name       to,
                      const asset      quantity,
                      const string     memo );

    /**
     * ## STATIC `get_rate`
     *
     * Get calculated rate (includes fee)
     *
     * ### params
     *
     * - `{name} contract` - contract account
     * - `{asset} quantity` - input quantity
     * - `{symbol_code} symcode` - out symbol code
     *
     * ### example
     *
     * ```c++
     * const asset quantity = asset{10000, symbol{"EOS", 4}};
     * const symbol_code symcode = symbol_code{"USDT"};
     * const asset rate = get_rate( "swap.sx"_n, quantity, symcode );
     * //=> "2.7712 USDT"
     * ```
     */
    static asset get_rate( const name contract, const asset quantity, const symbol_code symcode )
    {
        const asset fee = swapSx::get_fee( contract, quantity );
        return swapSx::get_price( contract, quantity - fee, symcode );
    }

    // convert
    static vector<double> get_uppers( const name contract, const symbol_code base_symcode, const symbol_code quote_symcode )
    {
        // settings
        swapSx::settings _settings( contract, contract.value );
        swapSx::tokens_table _tokens( contract, contract.value );
        const tokens_row base = _tokens.get( base_symcode.raw(), "base symbol code does not exists" );
        const tokens_row quote = _tokens.get( quote_symcode.raw(), "quote symbol code does not exists" );
        const int64_t amplifier = _settings.get().amplifier;

        // depth
        const double base_depth = swapSx::asset_to_double( base.depth );
        const double quote_depth = swapSx::asset_to_double( quote.depth );

        // ratio
        const double base_ratio = static_cast<double>(base.balance.amount) / base.depth.amount;
        const double quote_ratio = static_cast<double>(quote.balance.amount) / quote.depth.amount;

        // upper
        const double base_upper = ( amplifier * base_depth - base_depth + (base_depth * base_ratio));
        const double quote_upper = ( amplifier * quote_depth - quote_depth + (quote_depth * quote_ratio));

        return vector<double>{ base_upper, quote_upper };
    }

    static asset get_price( const name contract, const asset quantity, const symbol_code symcode )
    {
        // quantity input
        const double in_amount = swapSx::asset_to_double( quantity );

        // upper limits
        const vector<double> uppers = swapSx::get_uppers( contract, quantity.symbol.code(), symcode );
        const double base_upper = uppers[0];
        const double quote_upper = uppers[1];

        // Bancor V1 Formula
        const double out = swapSx::get_bancor_output( base_upper, quote_upper, in_amount );

        return swapSx::double_to_asset( out, get_symbol( contract, symcode ));
    }

    static asset get_fee( const name contract, const asset quantity )
    {
        // settings
        swapSx::settings _settings( contract, contract.value );
        const int64_t fee = _settings.get().fee;

        // fee colleceted from incoming transfer (in basis points 1/100 of 1% )
        asset calculated_fee = quantity * fee / 10000;

        // set minimum fee to smallest decimal of asset
        if ( fee > 0 && calculated_fee.amount == 0 ) calculated_fee.amount = 1;
        check( calculated_fee < quantity, "fee exceeds quantity");
        return calculated_fee;
    }

    // utils
    static double asset_to_double( const asset quantity )
    {
        if ( quantity.amount == 0 ) return 0.0;
        return quantity.amount / pow(10, quantity.symbol.precision());
    }
    static asset double_to_asset( const double amount, const symbol sym )
    {
        return asset{ static_cast<int64_t>(amount * pow(10, sym.precision())), sym };
    }

    // bancor
    static double get_bancor_output( const double base_reserve, const double quote_reserve, const double quantity )
    {
        const double out = (quantity * quote_reserve) / (base_reserve + quantity);
        if ( out < 0 ) return 0;
        return out;
    }

    static double get_bancor_input( const double quote_reserve, const double base_reserve, const double out )
    {
        const double inp = (base_reserve * out) / (quote_reserve - out);
        if ( inp < 0 ) return 0;
        return inp;
    }

    // tokens
    static name get_contract( const name contract, const symbol_code symcode )
    {
        return get_extended_symbol( contract, symcode ).get_contract();
    }

    static symbol get_symbol( const name contract, const symbol_code symcode )
    {
        return get_extended_symbol( contract, symcode ).get_symbol();
    }

    static extended_symbol get_extended_symbol( const name contract, const symbol_code symcode )
    {
        swapSx::tokens_table _tokens( contract, contract.value );
        auto token = _tokens.find( symcode.raw() );
        check( token != _tokens.end(), symcode.to_string() + " cannot find token");
        return extended_symbol{ token->sym, token->contract };
    }

    // action wrappers
    using setparams_action = eosio::action_wrapper<"setparams"_n, &swapSx::setparams>;
    using token_action = eosio::action_wrapper<"token"_n, &swapSx::token>;
    using log_action = eosio::action_wrapper<"log"_n, &swapSx::log>;

private:
    // utils
    symbol_code parse_memo_symcode( const string memo );
    void self_transfer( const name to, const asset quantity, const string memo );

    // tokens
    void add_depth( const asset quantity );
    void sub_depth( const asset quantity );
    bool is_token_exists( const symbol_code symcode );

    void check_is_active( const symbol_code symcode, const name contract );
    void check_max_ratio( const symbol_code symcode );
    void check_min_ratio( const asset out );

    double get_ratio( const symbol_code symcode );
    asset get_balance( const symbol_code symcode );
    asset get_depth( const symbol_code symcode );

    void set_balance( const symbol_code symcode );
    void add_balance( const asset quantity );
    void sub_balance( const asset quantity );

    // volume
    void update_volume( const vector<asset> volumes, const asset fee );

    // spot prices
    void update_spot_prices();
    double get_spot_price( const symbol_code base, const symbol_code quote );

    // settings
    void erase_all_tokens();
};
