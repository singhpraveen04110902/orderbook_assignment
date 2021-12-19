#include "NewOrderReject.h"

NewOrderReject::NewOrderReject(OrderType t,OrderSide s,double limit_price,uint64_t qty,Client *client,uint64_t cl_order_id) :
orderid(0),client_order_id(cl_order_id),side(s),type(t),orderPrice(limit_price),order_qty(qty)
{}

 char NewOrderReject::get_message_type() const 
{
    return msg_type;
}