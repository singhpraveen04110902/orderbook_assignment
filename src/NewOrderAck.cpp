#include "NewOrderAck.h"

NewOrderAck::NewOrderAck(const Order* new_order) :
orderid(new_order->orderid),client_order_id(new_order->client_order_id),side(new_order->side),type(new_order->type),orderPrice(new_order->orderPrice),order_qty(new_order->order_qty)
{}

 char NewOrderAck::get_message_type() const 
{
    return msg_type;
}