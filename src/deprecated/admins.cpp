// @admin
[[eosio::action]]
void sx::setadmin( const name account, const bool enabled )
{
    require_auth( get_self() );

    check( is_account( account ), "[account] does not exists");

    auto admin = _admins.find( account.value );

    // add
    if ( enabled ) {
        check( admin == _admins.end(), "[account] is already admin");
        _admins.emplace( get_self(), [&]( auto& row ) {
            row.account = account;
        });
    // delete
    } else {
        check( admin != _admins.end(), "[account] admin does not exist");
        _admins.erase( admin );
    }
}

void sx::require_auth_or_admin( eosio::name owner )
{
    // admins
    for (auto admin : _admins) {
        if ( has_auth( admin.account )) return;
    }

    // get_self()
    if ( has_auth( get_self() )) return;

    // user
    check( has_auth( owner ), "missing authority of " + owner.to_string());
}