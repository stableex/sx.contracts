
/**
 * ## TABLE `signups`
 *
 * - `{name} account` - signed up account
 * - `{name} referrer` - referrer account
 * - `{time_point_sec} timestamp` - timestamp of last `signup`
 *
 * ### example
 *
 * ```json
 * {
 *   "account": "myaccount",
 *   "referrer": "myreferrer",
 *   "timestamp": "2020-01-16T00:00:00"
 * }
 * ```
 */
struct [[eosio::table("signups"), eosio::contract("sx")]] signups_row {
    name                account;
    name                referrer;
    time_point_sec      timestamp;

    uint64_t primary_key() const { return account.value; }
};
typedef eosio::multi_index< "signups"_n, signups_row > signups_table;
