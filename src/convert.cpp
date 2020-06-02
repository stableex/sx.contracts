double stable::calculate_out( const asset quantity, const asset base, const asset quote )
{
    // validate incoming & outgoing
    const symbol_code in_symcode = quantity.symbol.code();

    // convert to double
    const double base_num = asset_to_double( base );
    const double quote_num = asset_to_double( quote );
    const double in_amount = asset_to_double( quantity );

    // in case quantity is the same as out_symbol
    if ( base.symbol.code() == quote.symbol.code() ) return in_amount;

    // Bancor V1 Formula
    return in_amount / ( base_num + in_amount ) * quote_num;
}

asset stable::calculate_fee( const asset quantity )
{
    stable::settings _settings( get_self(), get_self().value );
    const auto settings = _settings.get();

    // fee colleceted from incoming transfer (in basis points 1/100 of 1% )
    asset calculated_fee = quantity * settings.fee / 10000;

    // set minimum fee to smallest decimal of asset
    if ( settings.fee > 0 && calculated_fee.amount == 0 ) calculated_fee.amount = 1;
    check( calculated_fee < quantity, "fee exceeds quantity");
    return calculated_fee;
}

// asset stable::calculate_out( const asset quantity, const symbol_code symcode )
// {
//     const auto settings = _settings.get();
//     const symbol_code base_symcode = quantity.symbol.code();
//     const symbol_code quote_symcode = symcode;
//     check( base_symcode != quote_symcode, symcode.to_string() + " cannot convert symbol code to self");
//     check( quantity.symbol.raw() != 0, "[quantity] cannot be empty");
//     check( symcode.raw() != 0, "[symcode] cannot be empty");

//     // pools
//     auto base = _pools.find( base_symcode.raw() );
//     auto quote = _pools.find( quote_symcode.raw() );
//     const symbol quote_sym = quote->id.get_symbol();

//     // pegged
//     const double base_pegged = asset_to_double( base->pegged );
//     const double quote_pegged = asset_to_double( quote->pegged );

//     // asserts
//     check( base != _pools.end(), base_symcode.to_string() + " pool does not exist" );
//     check( quote != _pools.end(), quote_symcode.to_string() + " pool does not exist" );
//     check( base->balance.amount != 0, base_symcode.to_string() + " pool has no balance" );
//     check( quote->balance.amount != 0, quote_symcode.to_string() + " pool has no balance" );
//     check( base->depth.amount != 0, base_symcode.to_string() + " pool has no depth" );
//     check( quote->depth.amount != 0, quote_symcode.to_string() + " pool has no depth" );
//     check( base->connectors.find( quote_symcode )->raw() == quote_symcode.raw(), quote_symcode.to_string() + " connector does not exist" );
//     check( base->enabled, base_symcode.to_string() + " pool is not enabled" );
//     check( quote->enabled, quote_symcode.to_string() + " pool is not enabled" );

//     // depth
//     const double base_depth = asset_to_double( base->depth ) * base_pegged;
//     const double quote_depth = asset_to_double( quote->depth ) * quote_pegged;
//     const double min_depth = std::min( base_depth, quote_depth );

//     // min amplifier
//     const int64_t min_amplifier = std::min( base->amplifier, quote->amplifier );

//     // ratio
//     const double base_ratio = static_cast<double>(base->balance.amount) / base->depth.amount;
//     const double quote_ratio = static_cast<double>(quote->balance.amount) / quote->depth.amount;

//     // upper
//     const double base_upper = ( min_amplifier * min_depth - min_depth + (min_depth * base_ratio));
//     const double quote_upper = ( min_amplifier * min_depth - min_depth + (min_depth * quote_ratio));

//     // bancor
//     // amount / (balance_from + amount) * balance_to
//     const double in_amount = asset_to_double( quantity ) * base_pegged;
//     const double out_amount = in_amount / ( base_upper + in_amount ) * quote_upper;

//     // print("\nbase_symcode: " + base_symcode.to_string() + "\n");
//     // print("quote_symcode: " + quote_symcode.to_string() + "\n");
//     // print("min_amplifier: " + to_string( min_amplifier ) + "\n");
//     // print("base_pegged: " + to_string( base_pegged ) + "\n");
//     // print("quote_pegged: " + to_string( quote_pegged ) + "\n");
//     // print("base->balance.amount: " + to_string( base->balance.amount ) + "\n");
//     // print("quote->balance.amount: " + to_string( quote->balance.amount ) + "\n");
//     // print("base_depth: " + to_string( base_depth ) + "\n");
//     // print("quote_depth: " + to_string( quote_depth ) + "\n");
//     // print("base_ratio: " + to_string( base_ratio ) + "\n");
//     // print("quote_ratio: " + to_string( quote_ratio ) + "\n");
//     // print("base_upper: " + to_string( base_upper ) + "\n");
//     // print("quote_upper: " + to_string( quote_upper ) + "\n");
//     // print("out_amount: " + to_string( out_amount ) + "\n");
//     // print("in_amount: " + to_string( in_amount ) + "\n");

//     return double_to_asset( out_amount / quote_pegged, quote_sym );
// }
