
/**
 * ## TABLE `referrers`
 *
 * - `{name} referrer` - referrer account
 * - `{asset} transaction_fee` - transaction fee (ex: 0.01 USD)
 * - `{optional<string>} website` - referrer website
 * - `{optional<string>} description` - referrer description
 *
 * ### example
 *
 * ```json
 * {
 *   "referrer": "myreferrer",
 *   "transaction_fee": "0.01 USD",
 *   "metadata_json": [
 *     {"key": "website", "value": "https://mysite.com"},
 *     {"key": "description", "value": "My referrer UI"}
 *   ]
 * }
 * ```
 */
struct [[eosio::table("referrers"), eosio::contract("sx")]] referrers_row {
    name                        referrer;
    asset                       transaction_fee;
    map<eosio::name, string>    metadata_json;

    uint64_t primary_key() const { return referrer.value; }
};
typedef eosio::multi_index< "referrers"_n, referrers_row > referrers_table;


    // @referrer
    [[eosio::action]]
    void signup( const name account, const optional<name> referrer );

    // @referrer
    [[eosio::action]]
    void setreferrer( const name referrer, const asset transaction_fee, const map<eosio::name, string> metadata_json );

    // @referrer
    [[eosio::action]]
    void delreferrer( const eosio::name referrer );

    // @referrer
    using setreferrer_action = eosio::action_wrapper<"setreferrer"_n, &sx::setreferrer>;
    using delreferrer_action = eosio::action_wrapper<"delreferrer"_n, &sx::delreferrer>;
    using signup_action = eosio::action_wrapper<"signup"_n, &sx::signup>;

    // referrers
    name get_referrer( const name account );
    asset get_referral_fee( const name referrer, const symbol sym );
