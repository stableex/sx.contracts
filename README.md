# `swap.sx`

> SX Liquidity Exchange using virtual reserves to amplify liquidity pool depth.

## Supported Tokens

| symbol              | website                     |
|---------------------|-----------------------------|
| USDT (Tether)       | https://tether.to           |
| EOSDT (Equilibrium) | https://eosdt.com           |
| USDB (Bancor)       | https://usdb.peg.network    |
| BNT (Bancor)        | https://www.bancor.network  |
| USDE (Pizza)	  	  | https://pizza.live          |
| BOID	  	          | http://boid.io              |
| DAPP (LiquidApps)	  | https://liquidapps.io       |
| DICE                | https://www.dice.one        |

## Trading Pairs

| **Exchange**   | **Tokens**                                |
|----------------|-------------------------------------------|
| `swap.sx`      | EOS, USDT, EOSDT, DAPP, BNT, BOID, DICE   |
| `stable.sx`    | EOS, USDT, EOSDT, USDB, USDE              |
| `eosdt.sx`     | EOS, USDT, EOSDT                          |

## Quickstart

### 1. Transfer supported token with `memo`

```bash
cleos transfer myaccount swap.sx "1.0000 EOS" "USDT"
```

### 2. [`sx.js`](https://github.com/stableex/sx.js) Javascript library

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

    // calculate price
    const quantity = "1.0000 EOS";
    const symcode = "USDT";
    const rate = get_rate( quantity, symcode, tokens, settings );
    // rate: 2.7712 USDT
})();
```

### 3. `get_rate` EOSIO smart contract

```c++
const asset quantity = asset{10000, symbol{"EOS", 4}};
const symbol_code symcode = symbol_code{"USDT"};
const asset rate = swap::get_rate( "swap.sx"_n, quantity, symcode );
//=> "2.7712 USDT"
```

## API

- [STATIC `get_rate`](#static-get_rate)
- [STATIC `get_fee`](#static-get_fee)
- [TABLE `settings`](#table-settings)
- [TABLE `tokens`](#table-tokens)
- [TABLE `volume`](#table-volume)
- [ACTION `receipt`](#action-receipt)
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
const asset rate = swap::get_rate( "swap.sx"_n, quantity, symcode );
//=> "2.7712 USDT"
```

## STATIC `get_fee`

Get calculated rate (includes fee)

### params

- `{name} contract` - contract account
- `{asset} quantity` - input quantity

### example

```c++
const asset quantity = asset{10000, symbol{"USDT", 4}};
const asset rate = get_fee( "stable.sx"_n, quantity );
//=> "0.0004 USDT"
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

## ACTION `receipt`

Receipt of transaction

- **authority**: `get_self()`

### params

- `{name} owner` - owner account
- `{name} action` - receipt action ("convert")
- `{list<asset>} assets` - assets involved in receipt

### example

```bash
cleos push action stable.sx receipt '["myaccount", "convert", ["1.0000 EOS", "2.7200 USDT"]]' -p stable.sx
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
