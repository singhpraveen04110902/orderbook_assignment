#include "Limit.h"
#include <limits>

Limit::Limit (double p,uint64_t qty,OrderType type,OrderSide side) : total_volume (qty)
{
   type == OrderType::MARKET && side == OrderSide::BUY ? limit_price = std::numeric_limits<double>::max() 
                                                       : type == OrderType::MARKET ? limit_price = 0 : limit_price = p;
}