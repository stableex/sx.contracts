void sx::setreferrer( const name referrer, const asset transaction_fee, const map<eosio::name, string> metadata_json )
{
    require_auth( referrer );
    check( is_account( referrer ), "[referrer] account does not exist");

    auto referrers_itr = _referrers.find( referrer.value );

    check( transaction_fee.symbol.code() == USD, "[transaction_fee] must be in USD");
    check( asset_to_double( transaction_fee ) <= 0.25, "[transaction_fee] must be below 0.25 USD");
    check( transaction_fee.symbol.precision() <= 4, "[transaction_fee] precision cannot execeed 4");
    check( transaction_fee.amount >= 0, "[transaction_fee] must be positive");

    const asset fixed_precision_fee = double_to_asset( asset_to_double( transaction_fee ), symbol{"USD", 4} );

    if (referrers_itr == _referrers.end()) {
        _referrers.emplace( get_self(), [&]( auto& row ) {
            row.referrer = referrer;
            row.transaction_fee = fixed_precision_fee;
            row.metadata_json = metadata_json;
        });
    } else {
        _referrers.modify( referrers_itr, get_self(), [&]( auto& row ) {
            row.transaction_fee = fixed_precision_fee;
            row.metadata_json = metadata_json;
        });
    }
}

void sx::delreferrer( const name referrer )
{
    require_auth( referrer );

    auto& referrers = _referrers.get( referrer.value, "[referrer] does not exist" );
    _referrers.erase( referrers );
}

void sx::signup( const name account, const optional<name> referrer )
{
    require_auth( account );

    if ( referrer->value) _referrers.get( referrer->value, "[referrer] does not exists" );
    auto signups_itr = _signups.find( account.value );

    if ( signups_itr == _signups.end() ) {
        _signups.emplace( get_self(), [&]( auto& row ) {
            row.account = account;
            row.referrer = name{ referrer->value };
            row.timestamp = current_time_point();
        });
    } else {
        _signups.modify( signups_itr, get_self(), [&]( auto& row ) {
            row.referrer = name{ referrer->value };
            row.timestamp = current_time_point();
        });
    }
}

name sx::get_referrer( const name account )
{
    auto signups = _signups.find( account.value );
    if ( signups != _signups.end() ) return signups->referrer;
    return get_self();
}

asset sx::get_referral_fee( const name referrer, const symbol sym )
{
    auto referrers_itr = _referrers.find( referrer.value );
    const double pegged = asset_to_double( get_pegged( sym.code() ));
    double fee;
    if ( referrers_itr != _referrers.end() ) fee = asset_to_double( referrers_itr->transaction_fee );
    else fee = 0;

    return double_to_asset( fee / pegged, sym );
}
