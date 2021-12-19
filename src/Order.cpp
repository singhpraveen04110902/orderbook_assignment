#include "Order.h"

uint64_t Order::next_exchange_order_id = 1;

Order::Order (OrderSide s,OrderType t,double p,uint64_t qty,Client *ptr,uint64_t cl_order_id  ) : orderid(++next_exchange_order_id),side(s),type(t)
                                                                                  ,orderPrice(p),order_qty(qty),client(ptr), client_order_id(cl_order_id) {}
                                                                             