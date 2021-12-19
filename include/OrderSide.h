#pragma once
#include <iostream>

enum class OrderSide { BUY=1,SELL=2,SSELL=3} ;

// std::ostream& operator << (std::ostream& out,const OrderSide &side)
// {
//      if (side == OrderSide::BUY) out << "Buy" ;
//      else out << "Sell";
//      return out;
// }