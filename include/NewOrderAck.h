#pragma once

#include "Message.h"
#include <cstdint>
#include "OrderType.h"
#include "OrderSide.h"
#include "Order.h"



struct NewOrderAck : Message {
    const char msg_type = 'A';
    uint64_t orderid;
    uint64_t client_order_id;
    OrderSide side;
    OrderType type;
    double orderPrice;
    uint64_t order_qty;
    NewOrderAck(const Order* new_order);
     char get_message_type() const override;
};