void sx::add_volume( const asset volume, const asset proceeds )
{
    const name ram_payer = get_self();

    // variables
    const symbol_code symcode = volume.symbol.code();
    const time_point_sec timestamp = time_point_sec( (current_time_point().sec_since_epoch() / 86400) * 86400 );
    const auto volume_itr = _volume.find( timestamp.sec_since_epoch() );
    const auto pool = _pools.get( symcode.raw(), "[add_volume::symcode] pool does not exists");

    // create entry
    if ( volume_itr == _volume.end() ) {
        _volume.emplace( ram_payer, [&]( auto& row ){
            row.timestamp = timestamp;
            row.volume[symcode] = volume;
            row.proceeds[symcode] = proceeds;
        });
    }
    // modify entry
    else {
        _volume.modify( volume_itr, same_payer, [&]( auto& row ) {
            if (row.volume[ symcode ].symbol.code() == symcode ) {
                row.volume[symcode] += volume;
                row.proceeds[symcode] += proceeds;
            } else {
                row.volume[symcode] = volume;
                row.proceeds[symcode] = proceeds;
            }
        });
    }
    update_total_volume( timestamp );
}

void sx::update_total_volume( const time_point_sec timestamp )
{
    const auto itr = _volume.find( timestamp.sec_since_epoch() );

    asset total_volume = asset{0, SYM_USD };
    asset total_proceeds = asset{0, SYM_USD };

    for ( const auto [ symcode, volume ] : itr->volume ) {
        if ( !pool_exists( symcode )) continue;
        if ( symcode == USD ) continue;
        const double pegged = asset_to_double( get_pegged( symcode ) );
        total_volume += double_to_asset( asset_to_double( volume ) * pegged, SYM_USD );
    }

    for ( const auto [ symcode, proceeds ] : itr->proceeds ) {
        if ( !pool_exists( symcode )) continue;
        if ( symcode == USD ) continue;
        const double pegged = asset_to_double( get_pegged( symcode ) );
        total_proceeds += double_to_asset( asset_to_double( proceeds ) * pegged, SYM_USD );
    }

    _volume.modify( itr, same_payer, [&]( auto& row ) {
        row.volume[USD] = total_volume;
        row.proceeds[USD] = total_proceeds;
    });
}