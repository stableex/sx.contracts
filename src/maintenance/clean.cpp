void sx::clean( const eosio::name table, const std::optional<eosio::name> scope )
{
    require_auth( get_self() );

    if (table == "proceeds"_n) {
        auto proceeds_itr = _proceeds.begin();
        while ( proceeds_itr != _proceeds.end() ) {
            proceeds_itr = _proceeds.erase(proceeds_itr);
        }
    }
    else if (table == "settings"_n) _settings.remove();
}