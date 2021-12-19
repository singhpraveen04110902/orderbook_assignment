#pragma once
#include "Message.h"
#include <cstdint>

class Client {
  static uint64_t next_client_id;
  public :
    virtual void receiveMessage(const Message& msg) const = 0;
};
