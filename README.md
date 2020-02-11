# SX (Stable Token Liquidity Exchange)

> Low Fees, Narrow Spread & High Liquidity

## USDⓈ pairs

| symbol  | name          | website                  |
|---------|---------------|--------------------------|
| USDT    | Tether        | https://tether.to        |
| EOSDT   | Equilibrium   | https://eosdt.com        |
| EOS     | EOS           | https://eos.io           |

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

# Referrals

SX has an easy to implement referral system which allows any referrer to signup for free and start earning fees from each trade that occurs using their platform.

### Step 1 (register referral)

Referrers must initiate the `setreferrer` ACTION to be registered.

- `{name} name` - name of referrer
- `{asset} transaction_fee` - transaction fee based in USD (ex: `0.01 USD`)
- `{map<eosio::name, string>} metadata_json` -  (optional) Metadata JSON (key/value pairs)

```bash
cleos push stablestable setreferrer '["myreferral", "0.01 USD", [{"key":"website", "value": "https://myreferrer.com"}]]' -p myreferral
```

### Step 2-A (include in transfer `memo`)

Referrers can append their referrer account name to the token transfer.

### `memo` schema

```bash
"{symbol_code},{referrer_name}"
```

### `example`

```bash
cleos transfer myaccount stablestable "1.0000 USDT" "USDB,myreferrer" --contract tethertether
```

### Step 2-B (include `signup` action)

Referrers can include the `signup` action prior to the token transfer with the `account` as the authority.

- `{name} account` - account name
- `{name} referral` - name of referral

```bash
cleos push stablestable signup '["myaccount", "myreferral"]' -p myaccount
```

### **Referral Notes

- maximum transaction fee of `0.25 USD` per trade (lower fee could attract more traders).
- referrers will continue to earn fees even if their referred users does not include the referral memo.
- users can change their referral at anytime by including a different referral memo or using the `signup` action.
- both `convert` & `autoconvert` will earn fees for referrers.
- fees are based on the token being traded by the user, not the token being recieved
