# SX (Stable Token Liquidity Exchange)

> Low Fees, Narrow Spread & High Liquidity

## USDⓈ pairs

| symbol  | name          | website                   |
|---------|---------------|---------------------------|
| EOS     | EOS           | https://eos.io            |
| USDT    | Tether        | https://tether.to         |
| EOSDT   | Equilibrium   | https://eosdt.com         |
| USDE    | Pizza USDEOS  | https://pizza.live        |
| EBTC    | BitPie BTC    | https://eosstablecoin.com |

# Table of Contents

- [traders](#traders)
- [referrals](#referrals)
- [miners](#miners)
- [liquidity-providers](#liquidity-providers)
- [price-algo](#price-algo)

# Traders

SX is a liquidity exchange that strictly supports only highly available stable tokens as it's underlaying basket of liquidity pools. Using the concept of virtual reserves, SX can amplifify the liquidity pools which in result offers much better spreads for the traders on supported trading pairs.

## Step 1 (how to trade)

Send any of the supported stable token asset to the smart contract (`stablestable`) and include in the memo which asset you would like in return by providing the token `symbol_code` (ex: "`USDT`").

```bash
$ cleos transfer myaccount stablestable "1.0000 USDT" "EOSDT" --contract tethertether
```

## Step 2 (receipt)

Once a trade is completed, the account will be notified with a `receipt` ACTION which shows confirmation of the trade.
