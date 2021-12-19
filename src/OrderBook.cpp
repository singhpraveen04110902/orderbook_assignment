#include "OrderBook.h"
#include "Order.h"
#include "NewOrderAck.h"
#include "NewOrderReject.h"
#include "CancelAck.h"
#include "CancelReject.h"
#include "Execution.h"
#include <iostream>
#include <iomanip>
#include <limits>

OrderBook::OrderBook(uint16_t lSize, double bPrice) : lotSize(lSize), special_quotation_price(bPrice) {}

void OrderBook::onNewOrder(OrderType type, OrderSide side, double limit_price, uint64_t order_qty, Client *client, uint64_t cl_order_id)
{
    if (!is_valid_order(type, side, limit_price, order_qty, client))
    {
        sendOrderReject(type, side, limit_price, order_qty, client, cl_order_id);
        return;
    }
    add_order_to_book(type, side, limit_price, order_qty, client, cl_order_id);
}

void OrderBook::onCancel(uint64_t order_id, Client *client)
{
    auto it = available_orders.find(order_id);
    if (it == available_orders.end())
    {
        sendCancelReject(order_id, client);
        return;
    }
    if (it->second->side == OrderSide::BUY)
        buyTree.remove_order(it->second);
    else
        sellTree.remove_order(it->second);
    sendCancelAck(it->second);
    delete it->second;
    available_orders.erase(it);
}

bool OrderBook::is_valid_order(OrderType type, OrderSide side, double limit_price, uint64_t order_qty, Client *client)
{
    if (type != OrderType::LIMIT && type != OrderType::MARKET)
        return false;
    if (side != OrderSide::BUY && side != OrderSide::SELL)
        return false;
    if (type == OrderType::LIMIT && limit_price <= 0)
        return false;
    if (order_qty <= 0 || order_qty % lotSize)
        return false;
    if (!client)
        return false;
    return true;
}

void OrderBook::add_order_to_book(OrderType type, OrderSide side, double limit_price, uint64_t order_qty, Client *client, uint64_t cl_order_id)
{
    Order *new_order = new (std::nothrow) Order(side, type, limit_price, order_qty, client, cl_order_id);
    if (!new_order)
    {
        sendOrderReject(type, side, limit_price, order_qty, client, cl_order_id);
        return;
    }
    if (side == OrderSide::BUY)
    {
        if (!buyTree.add_new_order(new_order))
        {
            sendOrderReject(type, side, limit_price, order_qty, client, cl_order_id);
            return;
        }
    }
    else
    {
        if (!sellTree.add_new_order(new_order))
        {
            sendOrderReject(type, side, limit_price, order_qty, client, cl_order_id);
            return;
        }
    }
    auto pair = available_orders.insert({new_order->orderid, new_order});
    if (!pair.second)
    {
        sendOrderReject(type, side, limit_price, order_qty, client, cl_order_id);
        return;
    }
    sendOrderAck(new_order);
    check_cross_with_source_side(side);
}

void OrderBook::sendOrderAck(const Order *new_order)
{
    NewOrderAck ack(new_order);
    new_order->client->receiveMessage(ack);
}

void OrderBook::sendOrderReject(OrderType type, OrderSide side, double limit_price, uint64_t order_qty, Client *client, uint64_t cl_order_id)
{
    NewOrderReject reject(type, side, limit_price, order_qty, client, cl_order_id);
    client->receiveMessage(reject);
}

void OrderBook::sendCancelAck(const Order *orignal_order)
{
    CancelAck cack(orignal_order);
    orignal_order->client->receiveMessage(cack);
}

void OrderBook::sendCancelReject(uint64_t orderid, Client *client)
{
    CancelReject creject(orderid);
    client->receiveMessage(creject);
}

double OrderBook::get_bestBid()
{
    return buyTree.get_best_price();
}

double OrderBook::get_bestAsk()
{
    return sellTree.get_best_price();
}

void OrderBook::check_cross_with_source_side(OrderSide side)
{
    auto buy_start_iterator = buyTree.begin();
    auto buy_end_iterator = buyTree.end();
    auto sell_start_iterator = sellTree.begin();
    auto sell_end_iterator = sellTree.end();
    while (buy_start_iterator != buy_end_iterator && sell_start_iterator != sell_end_iterator && buy_start_iterator->first->limit_price >= sell_start_iterator->first->limit_price)
    {
        auto first_buy_order = buy_start_iterator->first->first_order;
        auto first_sell_order = sell_start_iterator->first->first_order;

        if (side == OrderSide::BUY && first_sell_order->type == OrderType::MARKET)
        {
            sendExecution(first_buy_order, first_sell_order, special_quotation_price, buy_start_iterator, sell_start_iterator);
        }
        else if (side == OrderSide::BUY && first_sell_order->type == OrderType::LIMIT)
        {
            sendExecution(first_buy_order, first_sell_order, get_bestAsk(), buy_start_iterator, sell_start_iterator);
        }
        else if (side == OrderSide::SELL && first_buy_order->type == OrderType::MARKET)
        {
            sendExecution(first_buy_order, first_sell_order, special_quotation_price, buy_start_iterator, sell_start_iterator);
        }
        else
        {
            sendExecution(first_buy_order, first_sell_order, get_bestBid(), buy_start_iterator, sell_start_iterator);
        }

        if (!first_buy_order->order_qty)
        {
            buyTree.remove_order(first_buy_order);
            available_orders.erase(first_buy_order->orderid);
            delete first_buy_order;
            buy_start_iterator = buyTree.begin();
            buy_end_iterator = buyTree.end();
        }
        if (!first_sell_order->order_qty)
        {
            sellTree.remove_order(first_sell_order);
            available_orders.erase(first_sell_order->orderid);
            delete first_sell_order;
            sell_start_iterator = sellTree.begin();
            sell_end_iterator = sellTree.end();
        }
    }
}

void OrderBook::sendExecution(Order *sourceOrder, Order *otherOrder, double execPrice, limitTreeIterator buyLimitIterator, limitTreeIterator selllimitTreeIterator)
{
    uint64_t execQty = std::min(sourceOrder->order_qty, otherOrder->order_qty);
    sourceOrder->order_qty -= execQty;
    otherOrder->order_qty -= execQty;
    Execution e1(sourceOrder->orderid, sourceOrder->client_order_id, sourceOrder->side, sourceOrder->type, execPrice, execQty);
    Execution e2(otherOrder->orderid, otherOrder->client_order_id, otherOrder->side, otherOrder->type, execPrice, execQty);
    sourceOrder->client->receiveMessage(e1);
    otherOrder->client->receiveMessage(e2);
    buyLimitIterator->first->total_volume -= execQty;
    selllimitTreeIterator->first->total_volume -= execQty;
    special_quotation_price = execPrice;
}

void OrderBook::printOrderBook()
{
    std::cout << "====== OrderBook SnapShot ======\n";
    std::cout << "Bid" << std::right << std::setw(20) << "Ask\n";
    printLimits(sellTree.crbegin(), sellTree.crend(), 23);
    printLimits(buyTree.cbegin(), buyTree.cend(), 0);
    std::cout << "=================================\n";
}

void OrderBook::printLimits(limitTreeIterator begin, limitTreeIterator end, std::size_t formatter)
{
    while (begin != end)
    {
        auto limit = begin->first;
        std::cout << std::left << std::setfill(' ') << std::setw(formatter);
        if (limit->limit_price == 0 || limit->limit_price == std::numeric_limits<double>::max())
            std::cout << 'M';
        else
            std::cout << limit->limit_price;
        std::cout << "(" << limit->total_volume << ")"
                  << "\n";
        auto order = limit->first_order;
        ++begin;
    }
}

void OrderBook::printLimits(limitTreeReverseIterator begin, limitTreeReverseIterator end, std::size_t formatter)
{
    while (begin != end)
    {
        auto limit = begin->first;
        std::cout << std::setw(formatter) << std::right;
        if (limit->limit_price == 0 || limit->limit_price == std::numeric_limits<double>::max())
            std::cout << 'M';
        else
            std::cout << limit->limit_price;
        std::cout << "(" << limit->total_volume << ")"
                  << "\n";
        auto order = limit->first_order;
        ++begin;
    }
}

void OrderBook::printDetailedBook()
{
    if (buyTree.size() == 0 && sellTree.size() == 0)
    {
        std::cout << "Book is empty \n";
        return;
    }
    if (buyTree.size())
    {
        std::cout << "All Orders at Bid Side \n";
        printDetailedBook(buyTree.cbegin(), buyTree.cend());

        std::cout << "\n";
    }
    if (sellTree.size())
    {
        std::cout << "All Orders at Ask Side \n";
        printDetailedBook(sellTree.cbegin(), sellTree.cend());
    }
}

void OrderBook::printDetailedBook(limitTreeIterator begin, limitTreeIterator end)
{
    while (begin != end)
    {
        auto limit = begin->first;
        auto first_order = limit->first_order;
        if (limit->limit_price == std::numeric_limits<double>::max() || limit->limit_price == 0)
            std::cout << "      "
                      << "limit Price : "
                      << "Market"
                      << "(Total_qty =" << limit->total_volume << ")"
                      << " -> ";
        else
            std::cout << "      "
                      << "limit Price : "
                      << limit->limit_price
                      << "(Total_qty =" << limit->total_volume << ")"
                      << " -> ";
        while (first_order)
        {
            std::cout << "exch_order_id:" << first_order->orderid << " order_qty:" << first_order->order_qty << " order_price:" << first_order->orderPrice << " ,";
            first_order = first_order->next;
        }
        std::cout << "\n";
        ++begin;
    }
}

  OrderBook::OrderBook(OrderBook &&other) noexcept :  buyTree (std::move(other.buyTree)), sellTree (std::move(other.sellTree)),
                                                      special_quotation_price (other.special_quotation_price), available_orders (std::move(other.available_orders))
  {   
     
  }
  OrderBook& OrderBook::operator=(OrderBook &&other) noexcept 
  {
       if (&other != this)
       {
           for (auto pair : available_orders) delete pair.second;
           buyTree = std::move(other.buyTree);
           sellTree = std::move(other.sellTree);
           available_orders = std::move(other.available_orders);
       }
       return *this;
  }

OrderBook::~OrderBook()
{
    for (auto it : available_orders)
        delete it.second;
}
