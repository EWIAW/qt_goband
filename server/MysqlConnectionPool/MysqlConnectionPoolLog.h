#pragma once
#include <iostream>

#define MYSQLLOG(str) \
    std::cout << __FILE__ << ":" << __LINE__ << " " << __TIMESTAMP__ << " : " << str << std::endl;