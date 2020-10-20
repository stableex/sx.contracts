#include <math.h>

symbol_code sx::swap::parse_memo_symcode( const string memo )
{
    const string error = "`memo` must be symbol code (ex: \"USDT\")";
    check( memo.length() > 0 && memo.length() <= 7 , error );

    // must be all uppercase alpha characters
    for (char const c: memo ) {
        check( isalpha(c) && isupper(c), error );
    }
    const symbol_code symcode = symbol_code{ memo };
    check( symcode.is_valid(), error );
    return symcode;
}

void sx::swap::self_transfer( const name to, const asset quantity, const string memo )
{
    token::transfer_action transfer( get_contract( get_self(), quantity.symbol.code() ), { get_self(), "active"_n });
    transfer.send( get_self(), to, quantity, memo );
}

[[eosio::action]]
void sx::swap::clear()
{
    require_auth( get_self() );

    sx::swap::settings _settings( get_self(), get_self().value );
    sx::swap::tokens _tokens( get_self(), get_self().value );

    _settings.remove();
    clear_table( _tokens );
}

template <typename T>
void sx::swap::clear_table( T& table )
{
    auto itr = table.begin();
    while ( itr != table.end() ) {
        itr = table.erase( itr );
    }
}

name sx::swap::get_contract( const name code, const symbol_code symcode )
{
    return get_extended_symbol( code, symcode ).get_contract();
}

symbol sx::swap::get_symbol( const name code, const symbol_code symcode )
{
    return get_extended_symbol( code, symcode ).get_symbol();
}

extended_symbol sx::swap::get_extended_symbol( const name code, const symbol_code symcode )
{
    sx::swap::tokens _tokens( code, code.value );
    auto token = _tokens.find( symcode.raw() );
    check( token != _tokens.end(), symcode.to_string() + " cannot find token");
    return extended_symbol{ token->sym, token->contract };
}