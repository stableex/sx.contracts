symbol_code sx::parse_memo_symcode( const vector<string> tokens )
{
    if ( tokens.size() == 0 ) return symbol_code{ 0 };
    // wildcard symbol (* or 888)
    if ( tokens[0] == "*" || tokens[0] == "888") return symbol_code{ 888 };
    const symbol_code symcode = symbol_code{ tokens[0] };
    check( symcode.is_valid(), "symbol code is not valid");
    return symcode;
}

double sx::asset_to_double( const eosio::asset quantity )
{
    if ( quantity.amount == 0 ) return 0.0;
    return quantity.amount / pow(10, quantity.symbol.precision());
}

eosio::asset sx::double_to_asset( const double amount, const eosio::symbol sym )
{
    return eosio::asset{ static_cast<int64_t>(amount * pow(10, sym.precision())), sym };
}

eosio::name sx::get_contract( const eosio::symbol_code symcode )
{
    return _pools.get( symcode.raw(), "[symcode] pool does not exist" ).id.get_contract();
}

vector<eosio::string> sx::split( const string str, const string delim )
{
    vector<eosio::string> tokens;
    if ( str.size() == 0 ) return tokens;

    size_t prev = 0, pos = 0;
    do
    {
        pos = str.find(delim, prev);
        if (pos == string::npos) pos = str.length();
        string token = str.substr(prev, pos-prev);
        if (token.length() > 0) tokens.push_back(token);
        prev = pos + delim.length();
    }
    while (pos < str.length() && prev < str.length());
    return tokens;
}

void sx::require_auth_or_self( eosio::name owner )
{
    // get_self()
    if ( has_auth( get_self() )) return;

    // user
    check( has_auth( owner ), "missing authority of " + owner.to_string());
}
