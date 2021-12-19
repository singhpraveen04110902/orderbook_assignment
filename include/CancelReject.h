#pragma once
#include "Message.h"
#include <cstdint>

struct CancelReject : Message {
   const char msg_type = 'H';
   uint64_t orderid;
   CancelReject (uint64_t ordid) ;
    char get_message_type() const override;
};