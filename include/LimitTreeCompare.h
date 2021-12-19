#pragma once
#include "OrderSide.h"
#include "Limit.h"


template<OrderSide side>
class LimitTreeCompare {
   public :
    bool operator()(const Limit *l1,const Limit *l2) const
    {
         if (side == OrderSide::SELL) return l1->limit_price < l2->limit_price;
         else return l1->limit_price > l2->limit_price;
    }
};