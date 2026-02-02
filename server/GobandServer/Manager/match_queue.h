// 匹配队列模块
#pragma once
#include <list>
#include <condition_variable>
#include <iostream>
#include <mutex>

// 匹配队列
class MatchQueue
{
private:
    std::list<uint64_t> _queue; // 匹配队列
    std::mutex _mutex;

public:
    size_t size();                     // 获取元素个数
    bool empty();                      // 判断队列是否为空
    void push(const uint64_t &data);   // 入队列
    bool pop(uint64_t &data);          // 出队列
    void remove(const uint64_t &data); // 移除指定数据
};