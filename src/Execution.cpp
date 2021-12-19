#include "Execution.h"

Execution::Execution(uint64_t ordid,uint64_t cl_order_id,OrderSide s,OrderType t,double eprice,uint64_t eqty) : 
orderid(ordid),client_order_id(cl_order_id),side(s),type(t),execPrice(eprice),execQty(eqty)
{}

 char Execution::get_message_type() const 
{
    return msg_type;
}