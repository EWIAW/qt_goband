#include "match_queue.h"

size_t MatchQueue::size() // 获取元素个数
{
    std::unique_lock<std::mutex> lock(_mutex);
    return _queue.size();
}

bool MatchQueue::empty() // 判断队列是否为空
{
    std::unique_lock<std::mutex> lock(_mutex);
    return _queue.empty();
}

void MatchQueue::push(const uint64_t &data) // 入队列
{
    std::unique_lock<std::mutex> lock(_mutex);
    _queue.push_back(data);
    return;
}

bool MatchQueue::pop(uint64_t &data) // 出队列
{
    std::unique_lock<std::mutex> lock(_mutex);
    if (_queue.empty() == true)
    {
        return false;
    }
    data = _queue.front();
    _queue.pop_front();
    return true;
}

void MatchQueue::remove(const uint64_t &data) // 移除指定数据
{
    std::unique_lock<std::mutex> lock(_mutex);
    _queue.remove(data);
    return;
}