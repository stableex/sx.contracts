# `swap.sx`

> SX Amplified Liquidity Pools

## Stable Tokens

| symbol              | website                     |
|---------------------|-----------------------------|
| EOS                 | https://eos.io              |
| USDT (Tether)       | https://tether.to           |
| PBTC (pTokens)      | http://ptokens.io           |
| EOSDT (Equilibrium) | https://eosdt.com           |
| USDB (Bancor)       | https://usdb.peg.network    |
| USDE (Pizza)	  	  | https://pizza.live          |
| VIGOR     	  	  | https://vigor.ai            |
| USN     	  	      | https://danchor.io          |

## Liquidity Pools

| **Pool**       | **Tokens**      |
|----------------|-----------------|
| `swap.sx`      | EOS, USDT, PBTC
| `stable.sx`    | EOS, USDT, EOSDT, USDB, VIGOR, USDE, USN
| `vigor.sx`     | VIGOR, USDT

## Quickstart

### `convert` @ cleos

```bash
cleos transfer myaccount swap.sx "1.0000 EOS" "USDT"
```

### `convert` @ smart contract

```c++
#include <eosio.token/eosio.token.hpp>

const asset amount_in = asset{10000, symbol{"EOS", 4}};
const symbol_code symcode_out = symbol_code{"USDT"};

// send inline action
token::transfer_action transfer( "eosio.token"_n, { get_self(), "active"_n });
transfer.send( get_self(), "swap.sx"_n, amount_in, symcode_out.to_string() );
```

### `get_amount_out` @ smart contract

```c++
// Inputs
const name contract = "swap.sx"_n;
const asset amount_in = asset{10000, symbol{"EOS", 4}};
const asset symcode_out = symbol_code{"USDT"};

// Calculation
const asset amount_out = swapSx::get_amount_out( contract, amount_in, symcode_out );
// => "2.7328 USDT"
```

### `tokens` @ smart contract

```c++
swapSx::tokens _tokens( "swap.sx"_n, "swap.sx"_n.value );
for ( const auto token : _tokens ) {
    token.sym;
    // => "4,USDT"
    token.contract;
    // => "tethertether"
}
```

## Table of Content

- [STATIC `get_amount_out`](#static-get_amount_out)
- [TABLE `settings`](#table-settings)
- [TABLE `tokens`](#table-tokens)
- [ACTION `setparams`](#action-setparams)
- [ACTION `token`](#action-token)
- [ACTION `swaplog`](#action-swaplog)

## STATIC `get_amount_out`

Given an input amount of an asset and pair reserves, returns the maximum output amount of the other asset

### params

- `{name} contract` - contract name
- `{asset} amount_in` - amount input
- `{asset} symcode_out` - symbol code out

### example

```c++
// Inputs
const name contract = "swap.sx"_n;
const asset amount_in = asset{10000, symbol{"EOS", 4}};
const asset symcode_out = symbol_code{"USDT"};

// Calculation
const asset amount_out = swapSx::get_amount_out( contract, amount_in, symcode_out );
// => "2.7328 USDT"
```

## TABLE `settings`

- `{int64_t} fee` - trading fee (pips 1/100 of 1%)
- `{int64_t} amplifier` - liquidity pool amplifier

### example

```json
{
    "fee": 20,
    "amplifier": 10
}
```

## TABLE `tokens`

- `{symbol} sym` -  token symbol
- `{contract} contract` - token contract account name
- `{asset} balance` - current balance
- `{asset} depth` - liquidity depth
- `{asset} reserve` - liquidity reserve
- `{asset} virtual_reserve` - virtual liquidity reserve

### example

```json
{
    "sym": "4,USDT",
    "contract": "tethertether",
    "balance": "10000.0000 USDT",
    "depth": "10000.0000 USDT",
    "reserve": "10000.0000 USDT",
    "virutal_reserve": "10000.0000 USDT"
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
cleos push action swap.sx setparams '[{"fee": 50, "amplifier": 20}]' -p swap.sx
```

## ACTION `swaplog`

Notify of trade

- **authority**: `get_self()`

### params

- `{name} buyer` - trader buyer account
- `{asset} quantity` - incoming quantity
- `{asset} rate` - outgoing rate
- `{asset} fee` - fee paid per trade

### example

```bash
cleos push action swap.sx swaplog '["myaccount", "3.0000 EOS", "7.0486 USDT", "0.0060 EOS"]' -p swap.sx
```
