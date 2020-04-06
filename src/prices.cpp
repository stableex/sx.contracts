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

asset sx::get_pegged( const symbol_code symcode )
{
    const auto& pool = _pools.get( symcode.raw(), "pool does not exists");
    asset pegged = pool.pegged;

    check( pegged.amount > 0, "pegged must be positive");

    return pegged;
}
