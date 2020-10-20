asset sx::swap::get_upper( const symbol_code symcode )
{
    // settings
    sx::swap::settings _settings( get_self(), get_self().value );
    sx::swap::tokens _tokens( get_self(), get_self().value );
    const tokens_row token = _tokens.get( symcode.raw(), "[get_upper] symbol code does not exists" );
    const int64_t amplifier = _settings.get().amplifier;

    // ratio
    const int64_t depth = token.depth.amount;
    const double ratio = static_cast<double>(token.balance.amount) / depth;
    const int64_t upper = amplifier * depth - depth + (depth * ratio);

    return asset{ upper, token.sym };
}

void sx::swap::set_virtual_reserve( const symbol_code symcode )
{
    sx::swap::tokens _tokens( get_self(), get_self().value );
    sx::swap::settings _settings( get_self(), get_self().value );

    check(_settings.exists(), "settings are not initialized");

    auto itr = _tokens.find( symcode.raw() );
    check(itr != _tokens.end(), "[set_virtual_reserve] symcode does not exists");

    _tokens.modify( itr, same_payer, [&]( auto & row ) {
        row.virtual_reserve = get_upper( symcode );
    });
}