#pragma once
#include <cstdint>
#include <OrderType.h>
#include <OrderSide.h>

class Order;
struct Limit {
    double limit_price = 0;
    uint64_t total_volume = 0;
    Order* first_order = nullptr;
    Order* last_order = nullptr;
    Limit (double p,uint64_t qty,OrderType type,OrderSide side);
};