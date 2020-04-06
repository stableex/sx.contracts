// @admin
[[eosio::action]]
void sx::setfee( const int64_t pool_fee, const asset transaction_fee, const optional<int64_t> stability_fee )
{
    require_auth( get_self() );

    auto settings = _settings.get();
    check( transaction_fee.symbol == SYM_USD, "[transaction_fee] must use 4 decimal USD symbol (ex: \"0.2500 USD\")");
    check( transaction_fee.amount <= 2500, "[transaction_fee] cannot exceed \"0.2500 USD\")");
    check( pool_fee <= 3000, "[pool_fee] cannot exceed 3%");
    check( pool_fee >= 0, "[pool_fee] must be positive");

    if ( stability_fee ) {
        check( settings.stability_fee != stability_fee || settings.pool_fee != pool_fee || settings.transaction_fee != transaction_fee , "[pool_fee] or [transaction_fee] or [stability_fee] was not modified");
        check( stability_fee <= 3000, "[stability_fee] cannot exceed 3%");
        check( stability_fee >= 0, "[stability_fee] must be positive");
        settings.stability_fee = stability_fee.value();
    } else {
        check( settings.pool_fee != pool_fee || settings.transaction_fee != transaction_fee , "[pool_fee] or [transaction_fee] was not modified");
    }
    settings.pool_fee = pool_fee;
    settings.transaction_fee = transaction_fee;
    _settings.set( settings, same_payer );
}
