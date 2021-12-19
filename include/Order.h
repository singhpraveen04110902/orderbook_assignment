#pragma once
#include <cstdint>
#include <string>

#include "OrderSide.h"
#include "OrderType.h"
#include "Message.h"
#include "Client.h"
#include "Limit.h"

struct Order  {
    static uint64_t next_exchange_order_id;
    uint64_t orderid;
    uint64_t client_order_id;
    OrderSide side;
    OrderType type;
    double orderPrice;
    uint64_t order_qty;
    Client *client = nullptr;
    Limit* parentLimit = nullptr;
    Order* next = nullptr;
    Order* prev = nullptr;

    Order (OrderSide s,OrderType t,double p,uint64_t qty,Client *ptr,uint64_t client_order_id ) ;
};
