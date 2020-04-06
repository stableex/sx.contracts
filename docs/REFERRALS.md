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
