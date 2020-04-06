/**
 * ## TABLE `apr`
 *
 * ### params
 *
 * - `{symbol_code} symcode` - daily periods (86400 seconds)
 * - `{int64_t} day` - daily APR (pips 1/100 of 1%)
 * - `{int64_t} week` - weekly APR (pips 1/100 of 1%)
 * - `{int64_t} month` - monthly APR (pips 1/100 of 1%)
 *
 * ### example
 *
 * ```json
 * {
 *   "symcode": "USDT",
 *   "day": 550,
 *   "week": 450,
 *   "month": 300
 * }
 * ```
 */
struct [[eosio::table("apr"), eosio::contract("sx")]] apr_row {
    symbol_code         symcode;
    int64_t             day;
    int64_t             week;
    int64_t             month;

    uint64_t primary_key() const { return symcode.raw(); }
};
typedef eosio::multi_index< "apr"_n, apr_row > apr_table;

int64_t sx::calculate_apr( const asset proceeds, const asset depth )
{
    if ( depth.amount > 0 ) return proceeds.amount * 365 * 10000 / depth.amount;
    return 0;
}
