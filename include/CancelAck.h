#pragma once
#include "Message.h"
#include <cstdint>
#include "OrderType.h"
#include "OrderSide.h"
#include "Order.h"

struct CancelAck : Message
{
    const char msg_type = 'G';
    uint64_t orderid;
    uint64_t client_order_id;
    OrderSide side;
    OrderType type;
    double orderPrice;
    uint64_t order_qty;
    CancelAck(const Order *new_order);
    char get_message_type() const override;
};