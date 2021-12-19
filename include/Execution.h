#pragma once
#include "Message.h"
#include <cstdint>
#include "OrderSide.h"
#include "OrderType.h"
#include "Client.h"


struct Execution : Message{
     const char msg_type = 'E';
    uint64_t orderid;
    uint64_t client_order_id;
    OrderSide side;
    OrderType type;
    double execPrice;
    uint64_t execQty;
    Client *client;
    Execution(uint64_t ordid,uint64_t cl_order_id,OrderSide s,OrderType t,double eprice,uint64_t eqty);
     char get_message_type() const override;
};