void sx::check_pool_thresholds( const asset in, const asset out )
{
    // pools
    const symbol_code in_symcode = in.symbol.code();
    const symbol_code out_symcode = out.symbol.code();
    const auto in_pool = _pools.get( in_symcode.raw(), "[in] pool does not exist" );
    const auto out_pool = _pools.get( out_symcode.raw(), "[out] pool does not exist" );

    // ratios
    int64_t adjusted_in_ratio = in_pool.balance.amount * 100'00 / ( in_pool.depth.amount - in.amount );
    int64_t adjusted_out_ratio = (out_pool.balance.amount - out.amount) * 100'00 / out_pool.depth.amount;

    check( adjusted_in_ratio <= 400'00, in_symcode.to_string() + " ratio cannot exceed 400%");
    check( adjusted_out_ratio >= 25'00, out_symcode.to_string() + " ratio cannot be below 25%");
}
