# SX (Stable Asset Liquidity Exchange)

> Low Fees, Narrow Spread & High Liquidity

## USDⓈ pairs

| symbol  | name          | website                   |
|---------|---------------|---------------------------|
| USDT    | Tether        | https://tether.to         |
| EOSDT   | Equilibrium   | https://eosdt.com         |
| USDB    | Bancor        | https://usdb.peg.network  |
| USN     | Danchor       | https://danchor.io        |
| USDE	  | Pizza	      | https://pizza.live        |

# Table of Contents

- [convert](#convert)
- [miners](#miners) (TO-DO)
- [liquidity-providers](#liquidity-providers) (TO-DO)
- [price-algo](#price-algo) (TO-DO)

# Convert

SX is a liquidity exchange that strictly supports only highly available stable tokens as it's underlaying basket of liquidity pools. Using the concept of virtual reserves, SX can amplifify the liquidity pools which in result offers much better spreads for the traders on supported trading pairs.

#### `transfer`

Send any of the supported stable token asset to the smart contract (`stablestable`) and include in the memo which asset you would like in return by providing the token `symbol_code` (ex: "`USDT`").

```bash
$ cleos transfer myaccount stablestable "1.0000 USDT" "EOSDT" --contract tethertether
```

#### `receipt`

Once a trade is completed, the account will be notified with a `receipt` ACTION which shows confirmation of the trade.

```json
{
   "action": "convert",
   "assets": [
      "196.2032 USDB",
      "198.5536 USDT"
   ],
   "owner": "myaccount"
}
```
