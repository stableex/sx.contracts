#include <delphioracle/delphioracle.hpp>

void delphioracle::set( const eosio::name scope, const uint64_t value )
{
    delphioracle::datapointstable _datapoints( "delphioracle"_n, scope.value );
    auto datapoints_itr = _datapoints.find( 0 );

    if (datapoints_itr == _datapoints.end()) {
        _datapoints.emplace( get_self(), [&]( auto& row ) {
            row.id = 0;
            row.owner = get_self();
            row.value = value;
            row.median = value;
        });
    } else {
        _datapoints.modify( datapoints_itr, get_self(), [&]( auto& row ) {
            row.owner = get_self();
            row.value = value;
            row.median = value;
        });
    }
}