asset sx::get_transaction_fee( const symbol sym )
{
    const double pegged = asset_to_double( get_pegged( sym.code() ));
    const double fee = asset_to_double( _settings.get().transaction_fee );

    return double_to_asset( fee / pegged, sym );
}