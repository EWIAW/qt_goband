// 时间轮类
#pragma once
#include <sys/timerfd.h>

#include <vector>
#include <memory>
#include <unordered_map>

#include "TimerTask.h"
#include "Logger.h"
#include "Channel.h"

// 秒级定时器
class TimerWheel
{
    using WeakTaskPtr = std::weak_ptr<TimerTask>;
    using SharedTaskPtr = std::shared_ptr<TimerTask>;

    static const uint32_t capacity = 60; // 时间轮的长度
public:
    TimerWheel(EventLoop *loop);
    ~TimerWheel();

    void TimerTaskAdd(uint64_t id, uint32_t delay_time, const OnTimerTask &cb); // 添加一个定时任务
    void TimerTaskRefresh(uint64_t id);                                         // 刷新定时任务
    void TimerTaskCancel(uint64_t id);                                          // 取消定时任务
    bool TimerTaskIsExist(uint64_t id);                                         // 判断定时任务还在不在，true代表在

    int GetTimerFd(); // 获取定时器的fd

private:
    void RemoveWeakPtrMap(uint64_t id); // 从_weak_ptr_map_中移除任务
    void WheelRunOne();                 // 时间轮向后走一步
    int ReadTimerFd();                  // 从定时器fd中读取超时次数
    void OnTime();                      // 定时器超时后，所执行的任务

    void TimerTaskAddInLoop(uint64_t id, uint32_t delay_time, const OnTimerTask &cb); // 添加定时任务 由 EventLoop来完成
    void TimerTaskRefreshInLoop(uint64_t id);                                         // 刷新定时任务 由 EventLoop来完成
    void TimerTaskCancelInLoop(uint64_t id);                                          // 取消定时任务 由 EventLoop来完成

private:
    int _timer_fd_;      // 通过timerfd_create系统调用创建定时器的返回值
    uint32_t _tick_;     // 定时器秒针所在的位置
    uint32_t _capacity_; // 定时器的长度

    std::vector<std::vector<SharedTaskPtr>> _wheel_;          // 时间轮
    std::unordered_map<uint64_t, WeakTaskPtr> _weak_ptr_map_; // 存储任务id和任务对象所对应的weak_ptr

    EventLoop *_loop_;                           // 该定时器所属那个EventLoop
    std::unique_ptr<Channel> _timer_fd_channel_; // 定时器fd的channel
};