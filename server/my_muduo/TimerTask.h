// 定时任务类，一个对象 代表 一个定时任务
#pragma once

#include <stdint.h>

#include <functional>

using OnTimerTask = std::function<void()>; // 时间到后，所需要执行的任务
using ReleaseTask = std::function<void()>; // 任务销毁后，需要做的资源清理任务，这里的资源清理是指 清理在TimerWheel时间轮类中，该任务的weak_ptr
class TimerTask
{
public:
    TimerTask(uint64_t id, uint32_t delay_time, const OnTimerTask &cb);
    ~TimerTask();

    uint32_t getDelayTime();
    void canceled(); // 取消任务
    void setReleaseTask(const ReleaseTask &cb);

private:
    uint64_t _id_;         // 任务id
    uint32_t _delay_time_; // 定时时间
    bool _is_cancel_;      // 任务是否被取消，true代表被取消，false反之

    OnTimerTask _on_timer_task_;
    ReleaseTask _release_task_;
};