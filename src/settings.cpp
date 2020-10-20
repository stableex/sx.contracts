[[eosio::action]]
void sx::swap::setparams( const optional<sx::swap::params> params )
{
    require_auth( get_self() );
    sx::swap::settings _settings( get_self(), get_self().value );

    // clear table if params is `null`
    if ( !params ) {
        _settings.remove();
        erase_all_tokens();
        return;
    }

    check( params->fee <= 300, "fee cannot be greater than 3%");
    check( params->fee >= 0, "fee must be positive");
    check( params->amplifier <= 500, "amplifier cannot be greater than 500x");
    check( params->amplifier >= 0, "amplifier must be positive");

    // cannot modify amplifier once set
    if ( _settings.exists() ) check( _settings.get().amplifier == params->amplifier, "amplifier cannot be modified");

    _settings.set( *params, get_self() );
}

[[eosio::action]]
void sx::swap::token( const symbol_code symcode, const optional<name> contract )
{
    require_auth( get_self() );

    sx::swap::tokens _tokens( get_self(), get_self().value );
    auto itr = _tokens.find( symcode.raw() );

    // delete if contract is null
    if ( itr != _tokens.end() && contract->value == 0 ) {
        _tokens.erase( itr );
        return;
    }

    check( itr == _tokens.end(), "token already exists");

    // validate symcode & contract
    check( contract->value, "contract is required");
    const asset supply = token::get_supply( *contract, symcode );
    const asset balance = token::get_balance( *contract, get_self(), symcode );
    if ( contract ) check( supply.amount > 0, "invalid supply");

    _tokens.emplace( get_self(), [&]( auto& row ) {
        row.sym = supply.symbol;
        row.contract = *contract;
        row.balance = balance;
        row.depth = balance;
        row.reserve = balance;
    });

    // define virtual reserve to prevent it from being defined as 0
    set_virtual_reserve( symcode );
}

void sx::swap::erase_all_tokens()
{
    sx::swap::tokens _tokens( get_self(), get_self().value );

    set<symbol_code> tokens;
    for ( auto row : _tokens ) {
        tokens.insert(row.sym.code());
    }

    for ( const symbol_code symcode : tokens ) {
        _tokens.erase( _tokens.find( symcode.raw() ));
    }
}