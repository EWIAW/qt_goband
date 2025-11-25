#pragma once

#include <iostream>
#include <string>

// 时间戳类
class Timestamp
{
public:
    Timestamp();
    explicit Timestamp(int64_t seconds);
    static Timestamp Now();        // 获取当前时间
    std::string To_String() const; // 将时间转换位字符串

private:
    int64_t _seconds_;
};