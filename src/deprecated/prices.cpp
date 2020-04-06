// @admin
[[eosio::action]]
void sx::setpegged( const symbol_code symcode, const asset pegged )
{
    require_auth( get_self() );

    check( pegged.symbol.code() == USD, "[pegged] symbol code must be USD");
    check( pegged.symbol.precision() == 4, "[pegged] precision must be 4");
    check( pegged.amount >= 0, "[pegged] must be positive");

    auto& pool = _pools.get( symcode.raw(), "[symcode] pool does not exist" );
    _pools.modify( pool, get_self(), [&]( auto & row ) {
        check( row.pegged != pegged, "nothing was modified");
        row.pegged = pegged;
    });
}

asset sx::get_delphioracle_value( const name scope )
{
    delphioracle::datapointstable _datapoints( "delphioracle"_n, scope.value );
    auto datapoints_itr = _datapoints.begin();
    check( datapoints_itr != _datapoints.end(), "cannot iterate over delphioracle");

    return asset{ static_cast<int64_t>(datapoints_itr->value), symbol{"USD", 4}};
}

asset sx::get_delphioracle_median( const name scope )
{
    delphioracle::datapointstable _datapoints( "delphioracle"_n, scope.value );
    auto datapoints_itr = _datapoints.begin();
    check( datapoints_itr != _datapoints.end(), "cannot iterate over delphioracle");

    return asset{ static_cast<int64_t>(datapoints_itr->median), symbol{"USD", 4}};
}

asset sx::set_pegged( const symbol_code symcode )
{
    const auto& pool = _pools.get( symcode.raw(), "[set_pegged::symcode] pool does not exists");
    const asset pegged = get_pegged( symcode );

    if ( pool.pegged != pegged ) {
        _pools.modify( pool, get_self(), [&]( auto& row ) {
            row.pegged = pegged;
        });
    }
    return pegged;
}

asset sx::get_pegged( const symbol_code symcode )
{
    const auto& pool = _pools.get( symcode.raw(), "[get_pegged::symcode] pool does not exists");
    asset pegged = pool.pegged;

    if ( pool.type == EOS ) pegged = get_delphioracle_median("eosusd"_n);
    else if ( pool.type == BTC ) pegged = get_delphioracle_median("btcusd"_n);

    check( pegged.amount > 0, "pegged must be positive");

    return pegged;
}
