[[eosio::action]]
void stable::setparams( const optional<stable::params> params )
{
    require_auth( get_self() );
    stable::settings _settings( get_self(), get_self().value );

    if ( !params ) return _settings.remove();

    check( params->fee <= 10, "fee cannot be greater than 0.1%");
    check( params->fee >= 0, "fee must be positive");
    _settings.set( *params, get_self() );
}

[[eosio::action]]
void stable::token( const symbol_code symcode, const optional<name> contract, const bool active )
{
    require_auth( get_self() );

    stable::tokens _tokens( get_self(), get_self().value );
    auto itr = _tokens.find( symcode.raw() );

    // add
    if ( itr == _tokens.end() ) {
        // validate contract
        check( contract->value, "contract is required");
        const asset supply = token::get_supply( *contract, symcode );
        if ( contract ) check( supply.amount > 0, "invalid supply");

        _tokens.emplace( get_self(), [&]( auto& row ){
            row.sym = supply.symbol;
            row.contract = *contract;
            row.active = active;
        });
    } else {
        // modify
        if ( contract->value ) {
            _tokens.modify( itr, get_self(), [&]( auto& row ) {
                check( row.active != active, "active was not modified" );
                row.active = active;
                if ( !active ) row.spot_prices = map<symbol_code, double>{};
            });
        } else {
            _tokens.erase( itr );
        }
    }
}