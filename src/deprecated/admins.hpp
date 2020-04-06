/**
 * ## TABLE `admins`
 *
 * - `{name} account` - admin account name
 *
 * ### example
 *
 * ```json
 * {
 *   "account": "myaccount"
 * }
 * ```
 */
struct [[eosio::table("admins"), eosio::contract("sx")]] admins_row {
    name     account;

    uint64_t primary_key() const { return account.value; }
};
typedef eosio::multi_index< "admins"_n, admins_row > admins_table;
