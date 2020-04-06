// @admin
[[eosio::action]]
void sx::init()
{
    require_auth( get_self() );
    const eosio::name ram_payer = get_self();

    check( !_settings.exists(), "contract already initialized" );

    _settings.set( _settings.get_or_default(), ram_payer );
}

// @admin
[[eosio::action]]
void sx::setparams( const sx_parameters params )
{
    require_auth( get_self() );

    auto settings = _settings.get();
    _settings.set( params, same_payer );
}

// @admin
[[eosio::action]]
void sx::pause( const bool paused )
{
    require_auth( get_self() );

    auto settings = _settings.get();
    settings.paused = paused;
    _settings.set( settings, same_payer );
}

// @admin
[[eosio::action]]
void sx::setfee( const int64_t pool_fee, const optional<int64_t> stability_fee )
{
    require_auth( get_self() );

    auto settings = _settings.get();
    check( pool_fee <= 3000, "[pool_fee] cannot exceed 3%");
    check( pool_fee >= 0, "[pool_fee] must be positive");

    if ( stability_fee ) {
        check( settings.stability_fee != stability_fee || settings.pool_fee != pool_fee, "[pool_fee] or [stability_fee] was not modified");
        check( stability_fee <= 3000, "[stability_fee] cannot exceed 3%");
        check( stability_fee >= 0, "[stability_fee] must be positive");
        settings.stability_fee = stability_fee.value();
    } else {
        check( settings.pool_fee != pool_fee, "[pool_fee] was not modified");
    }

    settings.pool_fee = pool_fee;
    _settings.set( settings, same_payer );
}

// @admin
[[eosio::action]]
void sx::setmin( const asset min_convert, const asset min_staked )
{
    require_auth( get_self() );

    auto settings = _settings.get();
    check( min_convert.amount >= 0, "[min_convert] must be positive");
    check( min_convert.symbol == min_staked.symbol, "[min_convert] & [min_taked] symbol precision mismatch");
    check( min_convert.symbol.code() == USD, "[min_convert] must be in USD");
    check( min_staked.symbol.code() == USD, "[min_staked] must be in USD");
    check( settings.min_staked != min_staked || settings.min_convert != min_convert, "[min_staked] or [min_convert] was not modified");

    settings.min_staked = min_staked;
    settings.min_convert = min_convert;
    _settings.set( settings, same_payer );
}
