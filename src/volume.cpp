void stable::update_volume( const vector<asset> volumes, const asset fee )
{
    volume_singleton _volume( get_self(), get_self().value );
    auto volume = _volume.get_or_default();

    // reset volume if new daily period
    const time_point_sec timestamp = time_point_sec( (current_time_point().sec_since_epoch() / 86400) * 86400 );
    if ( timestamp > volume.timestamp ) {
        volume.volume = map<symbol_code, asset>{};
        volume.fees = map<symbol_code, asset>{};
        volume.timestamp = timestamp;
    }

    // volume
    for ( const asset quantity : volumes ) {
        if ( volume.volume.find( quantity.symbol.code() ) != volume.volume.end() ) volume.volume[ quantity.symbol.code() ] += quantity;
        else volume.volume[ quantity.symbol.code() ] = quantity;
    }

    // fees
    if ( volume.fees.find( fee.symbol.code() ) != volume.fees.end() ) volume.fees[ fee.symbol.code() ] += fee;
    else volume.fees[ fee.symbol.code() ] = fee;

    _volume.set( volume, get_self() );
}