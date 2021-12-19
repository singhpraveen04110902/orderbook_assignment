#include "CancelReject.h"

CancelReject::CancelReject(uint64_t ordid) : orderid(ordid) {}

 char CancelReject::get_message_type() const 
{
    return msg_type;
}