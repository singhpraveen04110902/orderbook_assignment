#include "CancelAck.h"


CancelAck::CancelAck(const Order* orignal_order) :
orderid(orignal_order->orderid),client_order_id(orignal_order->client_order_id),side(orignal_order->side),type(orignal_order->type),orderPrice(orignal_order->orderPrice),order_qty(orignal_order->order_qty)
{}

 char CancelAck::get_message_type() const 
{
    return msg_type;
}