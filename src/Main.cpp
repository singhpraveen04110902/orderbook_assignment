#include <cstdint>
#include <iostream>
#include <sstream>

#include "LimitTree.h"
#include "Message.h"
#include "NewOrderAck.h"
#include "NewOrderReject.h"
#include "OrderBook.h"
#include "Client.h"
#include "OrderType.h"
#include "OrderSide.h"
#include "CancelAck.h"
#include "CancelReject.h"
#include "Execution.h"


uint64_t client_order_id = 1001;

struct NewClient : Client
{
    uint64_t client_id = 12001;
    void process(const NewOrderAck &order_ack) const
    {
        std::cout << "Received Ack for Order -> "
                  << "Client_Order_Id = " << order_ack.client_order_id << ": "
                  << "Exchange_order_Id = " << order_ack.orderid << ": "
                  << "OrderQty = " << order_ack.order_qty << ": \n";
    }

    void process(const NewOrderReject &order_reject) const
    {
        std::cout << "Received Reject for Order -> "
                  << "Client_Order_Id = " << order_reject.client_order_id << ": "
                  << "Exchange_order_Id = " << order_reject.orderid << ": "
                  << "OrderQty = " << order_reject.order_qty << ": \n";
    }

    void process(const CancelAck &cancel_ack) const
    {
        std::cout << "Received Cancel Ack for Order -> "
                  << "Client_Order_Id = " << cancel_ack.client_order_id << ": "
                  << "Exchange_order_Id = " << cancel_ack.orderid << ": "
                  << "OrderQty = " << cancel_ack.order_qty << ": \n";
    }

    void process(const CancelReject &cancel_reject) const
    {
        std::cout << "Received Cancel Reject for Order -> "
                  << "Exchange Order Id = " << cancel_reject.orderid << "\n";
    }

    void process(const Execution &execution) const
    {
        std::cout << "Received Execution -> "
                  << "Client_Order_Id = " << execution.client_order_id << ": "
                  << "Exchange_order_Id = " << execution.orderid << ": "
                  << "Exec Qty = " << execution.execQty << ": "
                  << "Exec Price = " << execution.execPrice << ": \n";
    }

    void processMessage(const Message &msg) const
    {
        if (msg.get_message_type() == 'A')
            process(dynamic_cast<const NewOrderAck &>(msg));
        else if (msg.get_message_type() == 'R')
            process(dynamic_cast<const NewOrderReject &>(msg));
        else if (msg.get_message_type() == 'G')
            process(dynamic_cast<const CancelAck &>(msg));
        else if (msg.get_message_type() == 'H')
            process(dynamic_cast<const CancelReject &>(msg));
        else if (msg.get_message_type() == 'E')
            process(dynamic_cast<const Execution &>(msg));
    }

public:
    void receiveMessage(const Message &msg) const override
    {
        processMessage(msg);
    }
};

void addNewOrder(std::istringstream &stream, Client *ptr, OrderBook &book)
{
    int type;
    stream >> type;
    OrderType ordertype;
    if (type == 1)
        ordertype = OrderType::MARKET;
    else if (type == 2)
        ordertype = OrderType::LIMIT;
    int side;
    stream >> side;
    OrderSide order_side;
    if (side == 1)
        order_side = OrderSide::BUY;
    else if (side == 2)
        order_side = OrderSide::SELL;
    double order_price;
    stream >> order_price;
    uint64_t order_qty;
    stream >> order_qty;
    uint64_t client_order_id;
    stream >> client_order_id;
    book.onNewOrder(ordertype, order_side, order_price, order_qty, ptr, client_order_id);
}

void cancelOrder(std::istringstream &stream, Client *ptr, OrderBook &book)
{
    uint64_t orderid;
    stream >> orderid;
    book.onCancel(orderid, ptr);
}


OrderBook prePopulate_OrderBook(Client *client)
{
    OrderBook order_book(1,43);
    order_book.onNewOrder(OrderType::LIMIT,OrderSide::BUY,43.8475,28,client,++client_order_id);
    order_book.onNewOrder(OrderType::LIMIT,OrderSide::BUY,43.8450,60,client,++client_order_id);
    order_book.onNewOrder(OrderType::LIMIT,OrderSide::BUY,43.8425,49,client,++client_order_id);
    order_book.onNewOrder(OrderType::LIMIT,OrderSide::BUY,43.8400,18,client,++client_order_id);
    order_book.onNewOrder(OrderType::LIMIT,OrderSide::BUY,43.8375,100,client,++client_order_id);

    order_book.onNewOrder(OrderType::LIMIT,OrderSide::SELL,43.8500,397,client,++client_order_id);
    order_book.onNewOrder(OrderType::LIMIT,OrderSide::SELL,43.8525,34,client,++client_order_id);
    order_book.onNewOrder(OrderType::LIMIT,OrderSide::SELL,43.8550,66,client,++client_order_id);
    order_book.onNewOrder(OrderType::LIMIT,OrderSide::SELL,43.8575,47,client,++client_order_id);
    order_book.onNewOrder(OrderType::LIMIT,OrderSide::SELL,43.8600,1,client,++client_order_id);
    return order_book;
}

void printHelp()
{
    std::cout << "Available Commands \n";
    std::cout<< "add : add OrderType OrderSide limit_price order_qty [client_order_id] \n"
             <<"      where OrderType = 1(Market),2(Limit)\n"
             <<"            OrderSide = 1(Buy),2(Sell) \n"
             <<"            limit_price= 0(market order) , valid price (limit order) \n"
             <<"            order_qty = valid order qty\n"
             <<"            client_order_id = unsigned int value (optional) \n"
             <<"       example : add 2 2 43.5 500 1 (add a new limit sell order at price 43.5 and qty 500 with cl_order_id = 1\n";
    std::cout <<"cancel : cancel exchange_order_id \n"
              <<"         where exchange_order_id is orderid returned by exchange in new order ack. Can be found using \"printd\" command as well\n";
    std::cout <<"quit : (no parameter) : exit the program \n";
    std::cout <<"ba : (no parameter) : print best ask \n";
    std::cout <<"bb : (no parameter) : print best bid \n";
    std::cout <<"load : (no parameter) : loads a predefined orderbook (for testing) \n";
    std::cout <<"print : (no parameter) : prints snapshot of order book\n";
    std::cout <<"printd : (no parameter) : prints detiled view of order book limit by limit and order by order \n";
}

int main(int argc ,const char* argv[])
{
    if (argc != 3) 
    {
       std::cout << " Missing parameters : Main lotSize basePrice \n";
       return 0;
    }
    int16_t lotSize = std::stoi(argv[1]);
    double basePrice = std::stod(argv[2]);
    OrderBook order_book(lotSize,basePrice);
    NewClient my_client;
    std::string line;
    std::cout << "Enter Command : ";
    while (std::getline(std::cin, line))
    {
        std::istringstream stream(line);
        std::string cmd;
        stream >> cmd;
        for (auto &c : cmd)
            c = std::toupper(c);
        if (cmd == "ADD")
            addNewOrder(stream, &my_client, order_book);
        else if (cmd == "PRINT")
            order_book.printOrderBook();
        else if (cmd == "CANCEL")
            cancelOrder(stream, &my_client, order_book);
        else if (cmd == "BB")
        {
            double best_bid = order_book.get_bestBid();
            if (best_bid == 0)
                std::cout << "Best Bid not available \n";
            else
                std::cout << "Best Bid =" << best_bid << "\n";
        }
        else if (cmd == "BA")
        {
            double bestAsk = order_book.get_bestAsk();
            if (bestAsk == 0)
                std::cout << "Best Ask not available \n";
            else
                std::cout << "Best Bid =" << bestAsk << "\n";
        }else if (cmd == "LOAD")
        {
            order_book = prePopulate_OrderBook(&my_client);
        }
        else if (cmd == "PRINTD")
        {
            order_book.printDetailedBook();
        }else if (cmd == "HELP")
        {
            printHelp();
        }else if (cmd == "QUIT")
            break;
        std::cout << "Enter Command : ";
    }
}
