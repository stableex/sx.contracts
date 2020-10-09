asset swapSx::get_upper( const symbol_code symcode )
{
    // settings
    swapSx::settings _settings( get_self(), get_self().value );
    swapSx::tokens _tokens( get_self(), get_self().value );
    const tokens_row token = _tokens.get( symcode.raw(), "symbol code does not exists" );
    const int64_t amplifier = _settings.get().amplifier;

    // ratio
    const int64_t depth = token.depth.amount;
    const double ratio = static_cast<double>(token.balance.amount) / depth;
    const int64_t upper = amplifier * depth - depth + (depth * ratio);

    return asset{ upper, token.sym };
}
