#pragma once

#include <unordered_map>
#include <map>
#include <limits>

#include "LimitTree.h"
#include "Client.h"
#include "Limit.h"
#include "LimitTreeCompare.h"
#include "Order.h"

class OrderBook final
{
public:
  // using buylimitTreeIterator = std::map<Limit *, Order *, LimitTreeCompare<OrderSide::BUY>>::iterator;
  // using selllimitTreeIterator = std::map<Limit *, Order *, LimitTreeCompare<OrderSide::SELL>>::iterator;
    using limitTreeIterator = std::map<Limit *, Order *>::iterator;
    using limitTreeReverseIterator = std::map<Limit *, Order *>::const_reverse_iterator;

private:
  LimitTree<OrderSide::BUY> buyTree;
  LimitTree<OrderSide::SELL> sellTree;
  double special_quotation_price ;
  uint16_t lotSize;
  std::unordered_map<uint64_t, Order *> available_orders;
  bool is_valid_order(OrderType type, OrderSide side, double limit_price, uint64_t order_qty, Client *client);
  void add_order_to_book(OrderType type, OrderSide side, double limit_price, uint64_t order_qty, Client *client, uint64_t cl_order_id);
  void sendOrderAck(const Order *new_order);
  void sendOrderReject(OrderType type, OrderSide side, double limit_price, uint64_t order_qty, Client *client, uint64_t cl_order_id);
  void sendCancelAck(const Order *orignal_order);
  void sendCancelReject(uint64_t ordid, Client *client);
  void check_cross_with_source_side(OrderSide side);
  void sendExecution(Order *sourceOrder, Order *otherOrder, double execPrice, limitTreeIterator buyLimitIterator, limitTreeIterator selllimitTreeIterator);
  void printLimits(const limitTreeIterator begin,const limitTreeIterator end,std::size_t formatter);
   void printLimits(const limitTreeReverseIterator begin,const limitTreeReverseIterator end,std::size_t formatter);
  void printDetailedBook(limitTreeIterator begin,limitTreeIterator end);
public:
  void onNewOrder(OrderType type, OrderSide side, double limit_price, uint64_t order_qty, Client *client, uint64_t cl_order_id);
  void onCancel(uint64_t order_id, Client *client);
  void printOrderBook();
  void printDetailedBook();
  double get_bestBid();
  double get_bestAsk();

  OrderBook(uint16_t lSize,double bPrice);
  OrderBook(const OrderBook& ) = delete;
  OrderBook& operator=(const OrderBook&) = delete;
  OrderBook(OrderBook &&other) noexcept = default;
  OrderBook& operator=(OrderBook &&other) noexcept = default;
  ~OrderBook();
};


