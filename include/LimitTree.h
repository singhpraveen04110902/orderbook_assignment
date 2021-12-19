#pragma once
#include "Limit.h"
#include "Order.h"
#include "LimitTreeCompare.h"
#include "OrderSide.h"
#include "NewOrderAck.h"
#include "NewOrderReject.h"

#include <map>
#include <list>
#include <unordered_map>
#include <iostream>
#include <iomanip>
#include <limits>

template <OrderSide side>
class LimitTree
{
    std::map<Limit *, Order *, LimitTreeCompare<side>> tree;
    std::unordered_map<double, Limit *> available_limits;

public:
    bool add_new_order(Order *new_order);
    void remove_order(Order *orignal_order);
    double get_best_price();
    typename std::map<Limit *, Order *, LimitTreeCompare<side>>::iterator begin();
    typename std::map<Limit *, Order *, LimitTreeCompare<side>>::iterator end();
    const typename  std::map<Limit *, Order *, LimitTreeCompare<side>>::iterator cbegin();
    const typename  std::map<Limit *, Order *, LimitTreeCompare<side>>::iterator cend();
    const typename  std::map<Limit *, Order *, LimitTreeCompare<side>>::const_reverse_iterator crbegin();
    const typename  std::map<Limit *, Order *, LimitTreeCompare<side>>::const_reverse_iterator crend();
    std::size_t size();
    LimitTree() = default;
    LimitTree(const LimitTree& ) = delete;
    LimitTree& operator=(const LimitTree&) = delete;
    LimitTree (LimitTree &&) noexcept = default;
    LimitTree& operator=(LimitTree &&) noexcept = default;
    ~LimitTree();
};

template <OrderSide side>
bool LimitTree<side>::add_new_order(Order *new_order)
{
    auto price = new_order->type == OrderType::MARKET && side == OrderSide::BUY ? std::numeric_limits<double>::max()
                                                                                : new_order->type == OrderType::MARKET ? 0 : new_order->orderPrice;
    auto it = available_limits.find(price);
    if (it == available_limits.end())
    {
        auto new_limit = new (std::nothrow) Limit(new_order->orderPrice, 0, new_order->type, new_order->side);
        if (!new_limit)
            return false;
        auto pair1 = available_limits.insert({new_limit->limit_price, new_limit});
        if (!pair1.second)
            return false;
        it = pair1.first;
        auto pair2 = tree.insert({new_limit, new_order});
        if (!pair2.second)
            return false;
    }
    new_order->parentLimit = it->second;
    if (it->second->first_order)
    {
        it->second->last_order->next = new_order;
        new_order->prev = it->second->last_order;
        it->second->last_order = new_order;
    }
    else
    {
        it->second->first_order = it->second->last_order = new_order;
    }
    it->second->total_volume += new_order->order_qty;
    return true;
}

template <OrderSide side>
void LimitTree<side>::remove_order(Order *orignal_order)
{
    auto parent_limit = orignal_order->parentLimit;

    //case 1: if this is head
    if (!orignal_order->prev)
    {
        parent_limit->first_order = orignal_order->next;
        if (orignal_order->next)
            orignal_order->next->prev = nullptr;
    }
    else if (orignal_order->prev && orignal_order->next)
    {
        //case 3: if this order is in between head and tail
        orignal_order->prev->next = orignal_order->next;
        orignal_order->next->prev = orignal_order->prev;
    }
    else
    {
        //case 2 : its tail
        orignal_order->prev->next = orignal_order->next;
        parent_limit->last_order = orignal_order->prev;
    }

    // check if this limit has any order
    if (!parent_limit->first_order)
    {
        //remove from all available limits;
        available_limits.erase(parent_limit->limit_price);
        //remove from limit tree
        tree.erase(parent_limit);
        // delete created limit
        delete parent_limit;
    }
    else
    {
        parent_limit->total_volume -= orignal_order->order_qty;
    }
}


template <OrderSide side>
double LimitTree<side>::get_best_price()
{
    auto it = tree.cbegin();
    if (it != tree.cend())
    {
        if (it->first->limit_price == 0 || it->first->limit_price == std::numeric_limits<double>::max())
        {
            if ((++it) != tree.cend())
                return it->first->limit_price;
        }
        else
            return it->first->limit_price;
    }
    return 0;
}

template <OrderSide side>
typename std::map<Limit *, Order *, LimitTreeCompare<side>>::iterator LimitTree<side>::begin()
{
    return tree.begin();
}

template <OrderSide side>
typename std::map<Limit *, Order *, LimitTreeCompare<side>>::iterator LimitTree<side>::end()
{
    return tree.end();
}

template <OrderSide side>
const typename  std::map<Limit *, Order *, LimitTreeCompare<side>>::iterator LimitTree<side>::cbegin()
{
    return tree.begin();
}

template <OrderSide side>
const typename  std::map<Limit *, Order *, LimitTreeCompare<side>>::iterator LimitTree<side>::cend()
{
    return tree.end();
}

template <OrderSide side>
const typename  std::map<Limit *, Order *, LimitTreeCompare<side>>::const_reverse_iterator LimitTree<side>::crbegin()
{
    return tree.crbegin();
}

template <OrderSide side>
const typename  std::map<Limit *, Order *, LimitTreeCompare<side>>::const_reverse_iterator LimitTree<side>::crend()
{
    return tree.crend();
}

template <OrderSide side>
std::size_t LimitTree<side>::size()
{
    return tree.size();
}

template <OrderSide side>
LimitTree<side>::~LimitTree()
{
    for (auto it : available_limits)
          delete it.second;
}
