# `swap.sx`

> SX Leveraged Liquidity Pools

## Stable Tokens

| symbol              | website                     |
|---------------------|-----------------------------|
| USDT (Tether)       | https://tether.to           |
| EOSDT (Equilibrium) | https://eosdt.com           |
| USDB (Bancor)       | https://usdb.peg.network    |
| USDE (Pizza)	  	  | https://pizza.live          |
| VIGOR     	  	  | https://vigor.ai            |

## Supported Tokens

| symbol              | website                     |
|---------------------|-----------------------------|
| BNT (Bancor)        | https://www.bancor.network  |
| BOID	  	          | http://boid.io              |
| DAPP (LiquidApps)	  | https://liquidapps.io       |
| DICE                | https://www.dice.one        |

## Trading Pairs

| **Exchange**   | **Tokens**                                |
|----------------|-------------------------------------------|
| `swap.sx`      | EOS, USDT, EOSDT, DAPP, BNT, BOID, DICE   |
| `eosdt.sx`     | EOS, USDT, EOSDT                          |
| `eosdt.sx`     | EOS, USDT, EOSDT, VIGOR                   |
| `stable.sx`    | EOS, USDT, EOSDT, USDB, USDE              |

## Quickstart

### 1a. `convert` - cleos token transfer + `memo`

```bash
cleos transfer myaccount swap.sx "1.0000 EOS" "USDT"
```

### 1b. `convert` - smart contract transfer + `memo`

```c++
#include <eosio.token/eosio.token.hpp>

const asset quantity = asset{10000, symbol{"EOS", 4}};
const symbol_code symcode = symbol_code{"USDT"};

// send inline action
token::transfer_action transfer( "eosio.token"_n, { get_self(), "active"_n });
transfer.send( get_self(), "swap.sx"_n, quantity, symcode.to_string() );
```

### 2a. `get_rate` - javascript [`sx.js`](https://github.com/stableex/sx.js)

```js
import { JsonRpc } from 'eosjs';
import { get_tokens, get_settings, get_rate } from "sxjs";

(async () => {
    // nodeos
    const rpc = new JsonRpc("https://eos.eosn.io", { fetch: require('node-fetch') });

    // settings (HTTP request)
    const code = "swap.sx";
    const tokens = await get_tokens( rpc, code );
    const settings = await get_settings( rpc, code );

    // calculate rate
    const quantity = "1.0000 EOS";
    const symcode = "USDT";
    const rate = get_rate( quantity, symcode, tokens, settings );
    // => 2.7712 USDT
})();
```

### 2b. `get_rate` - smart contract [`swap.sx.hpp`](https://github.com/stableex/sx.swap/blob/master/swap.sx.hpp)

```c++
#include "swap.sx.hpp"

// calculate rate
const asset quantity = asset{10000, symbol{"EOS", 4}};
const symbol_code symcode = symbol_code{"USDT"};
const asset rate = swapSx::get_rate( "swap.sx"_n, quantity, symcode );
// => "2.7712 USDT"
```

## Table of Content

- [STATIC `get_rate`](#static-get_rate)
- [TABLE `settings`](#table-settings)
- [TABLE `tokens`](#table-tokens)
- [TABLE `volume`](#table-volume)
- [TABLE `spotprices`](#table-spotprices)
- [TABLE `docs`](#table-docs)
- [ACTION `setparams`](#action-setparams)
- [ACTION `token`](#action-token)

## STATIC `get_rate`

Get calculated rate (includes fee)

### params

- `{name} contract` - contract account
- `{asset} quantity` - input quantity
- `{symbol_code} symcode` - out symbol code

### example

```c++
const asset quantity = asset{10000, symbol{"USDT", 4}};
const symbol_code symcode = symbol_code{"USDT"};
const asset rate = swapSx::get_rate("swap.sx"_n, quantity, symcode);
//=> "2.7712 USDT"
```

## TABLE `settings`

- `{int64_t} fee` - trading fee (pips 1/100 of 1%)
- `{int64_t} amplifier` - liquidity pool amplifier

### example

```json
{
    "fee": 20,
    "amplifier": 100
}
```

## TABLE `tokens`

- `{symbol} sym` -  token symbol
- `{contract} contract` - token contract account name
- `{asset} balance` - current balance
- `{asset} depth` - liquidity depth

### example

```json
{
    "sym": "4,USDT",
    "contract": "tethertether",
    "balance": "10000.0000 USDT",
    "depth": "10000.0000 USDT"
}
```

## TABLE `volume`

- `{time_point_sec} timestamp` - daily periods (86400 seconds)
- `{map<symbol_code, asset>} fees` - total fees collected
- `{map<symbol_code, asset>} volume` - total trading volume of assets

### example

```json
{
"timestamp": "2020-06-03T00:00:00",
"volume": [
        {"key": "EOSDT", "value": "25.000000000 EOSDT"},
        {"key": "USDT", "value": "100.0000 USDT"}
    ],
    "fees": [
        {"key": "EOSDT", "value": "0.100000000 EOSDT"},
        {"key": "USDT", "value": "0.4000 USDT"}
    ]
}
```

## TABLE `spotprices`

- `{time_point_sec} last_modified` - last modified timestamp
- `{symbol_code} fees` - base symbol code
- `{map<symbol_code, double>} quotes` - quotes prices calculated relative to base

### example

```json
{
    "last_modified": "2020-06-10T19:23:08",
    "base": "USDT",
    "quotes": [
        { "key": "EOS", "value": "2.73194051169110930" },
        { "key": "USDT", "value": "1.00000000000000000" },
        { "key": "EOSDT", "value": "0.99514989360433914" }
    ]
}
```

## TABLE `docs`

- `{string} url` - Documentation url

### example

```json
{
    "url": "https://github.com/stableex/sx.swap"
}
```

## ACTION `token`

Add/removes token

- **authority**: `get_self()`

### params

- `{symbol_code} symcode` - token symbol code
- `{name} [contract=null]` - token contract account name (if `null` delete symbol)

### example

```bash
cleos push action swap.sx token '["USDT", "tethertether"]' -p swap.sx
```

## ACTION `setparams`

Update contract parameters

- **authority**: `get_self()`

### params

- `{settings_params} settings` - settings parameters

### example

```bash
cleos push action swap.sx setparams '[{"fee": 10, "amplifier": 20}]' -p stable.sx
```
