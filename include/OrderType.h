#pragma once
#include <iostream>

enum class OrderType { MARKET=1 , LIMIT=2,MOC=3 };

// std::ostream& operator << (std::ostream& out,const OrderType &type)
// {
//      if (type == OrderType::LIMIT) out << "Limit" ;
//      else out << "Market";
//      return out;
// }