// ADMIN ONLY (TO BE REMOVED)
// @admin
[[eosio::action]]
void sx::migrate( const name type )
{
    require_auth( get_self() );

    if ( type == "settings"_n) {
        _settings.remove();
    }

    else if ( type == "accounts"_n ) {
        auto& account = _accounts.get( get_self().value, "self does not exists");

        list<asset> balances;
        for ( const auto [ symcode, balance ] : account.balances ) {
            if ( balance.amount > 0 ) {
                balances.push_back( balance );
            }
        }

        for ( const auto balance : balances ) {
            sub_balance( get_self(), balance );
            update_pool_ratio( balance.symbol.code() );
        }
        clear_empty_balances( get_self() );
    }

    // else if ( type == "volume"_n ) {
    //     auto itr = _v0_volume.begin();
    //     while ( itr != _v0_volume.end() ) {
    //         for ( const auto [ symcode, balance ] : itr->volume ) {
    //             if ( _pools.find( symcode.raw() ) != _pools.end() ) {
    //                 add_volume( balance, asset{0, balance.symbol } );
    //             }
    //         }
    //         for ( const auto [ symcode, balance ] : itr->proceeds ) {
    //             if ( _pools.find( symcode.raw() ) != _pools.end() ) {
    //                 add_volume( asset{0, balance.symbol }, balance );
    //             }
    //         }
    //         itr = _v0_volume.erase( itr );
    //     }
    // }

    else if ( type == "proceeds"_n ) {
        list<asset> proceeds;
        auto& proceed = _proceeds.get( get_self().value, "self does not exists");
        for ( const auto [ symcode, balance ] : proceed.balances ) {
            if ( balance.amount > 0 ) {
                proceeds.push_back( balance );
            }
        }

        for ( const auto balance : proceeds ) {
            sub_proceeds_owner( get_self(), balance );
            update_pool_ratio( balance.symbol.code() );
        }
        clear_empty_balances( get_self() );
    }
    else check( false, "nothing to migrate");

    // auto itr = _pools.begin();
    // while ( itr != _pools.end() ) {
    //     itr = _pools.erase( itr );
    // }

    // for ( const auto pool : _pools ) {
    //     _v1_pools.emplace( get_self(), [&]( auto & row ) {
    //         row.id = pool.id;
    //         row.balance = token::get_balance( pool.id.get_contract(), get_self(), pool.id.get_symbol().code() );
    //         row.depth = pool.depth;
    //         row.ratio = row.balance.amount * 100'00 / row.depth.amount;
    //         row.proceeds = pool.proceeds;
    //         row.amplifier = 100'00;
    //         row.type = pool.type;
    //         row.pegged = asset{1'0000, symbol{"USD", 4}};
    //         row.connectors = pool.connectors;
    //         row.enabled = pool.enabled;
    //         row.metadata_json = pool.metadata_json;
    //     });
    // }

    // for ( const auto account : _staked ) {
    //     for ( const auto [ symcode, balance ] : account.balances ) {
    //         sub_staked( account.owner, balance );
    //         add_balance( account.owner, balance );
    //     }
    // }

    // // remove stablestable from rows
    // auto proceeds_itr = _proceeds.find( get_self().value );
    // if (proceeds_itr != _proceeds.end()) _proceeds.erase( proceeds_itr );

    // auto accounts_itr = _accounts.find( get_self().value );
    // if (accounts_itr != _accounts.end()) _accounts.erase( accounts_itr );
}
