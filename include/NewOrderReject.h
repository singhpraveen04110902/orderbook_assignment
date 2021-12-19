#pragma once

#include "Message.h"
#include <cstdint>
#include "OrderType.h"
#include "OrderSide.h"
#include "Order.h"



struct NewOrderReject : Message {
    const char msg_type = 'R';
    uint64_t orderid;
    uint64_t client_order_id;
    OrderSide side;
    OrderType type;
    double orderPrice;
    uint64_t order_qty;
    NewOrderReject(OrderType t,OrderSide s,double limit_price,uint64_t qty,Client *client,uint64_t cl_order_id);
     char get_message_type() const override;
};