# `stable.sx`

> Liquidity Exchange using virtual reserves to amplify liquidity pool depth.

## USDⓈ tokens

| symbol  | name          | website                   |
|---------|---------------|---------------------------|
| USDT    | Tether        | https://tether.to         |
| EOSDT   | Equilibrium   | https://eosdt.com         |
| USDB    | Bancor        | https://usdb.peg.network  |
| USDE	  | Pizza	      | https://pizza.live        |

## Smart Contract

| chain | contract       | amplifier | pairs                   |
|-------|----------------|-----------|-------------------------|
| EOS   | `stable.sx`    | 20        | USDT, EOSDT, USDB, USDE |
| EOS   | `eosdt.sx`     | 100       | USDT, EOSDT             |
| EOS   | `swap.sx`      | 3         | EOS, USDT, DAPP, BNT    |
| EOS   | `stablestable` | 20        | USDT, EOSDT, USDB, USDE |

## Quickstart

```bash
cleos transfer myaccount stable.sx "1.0000 EOS" "USDT"
```

## Build

```bash
$ eosio-cpp stable.sx.cpp -I include
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
const symbol_code symcode = symbol_code{"EOSDT"};
const asset rate = get_rate( "stable.sx"_n, quantity, symcode );
//=> "1.002990000 EOSDT"
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
