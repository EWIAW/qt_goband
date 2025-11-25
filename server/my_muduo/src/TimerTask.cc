#include "TimerTask.h"

TimerTask::TimerTask(uint64_t id, uint32_t delay_time, const OnTimerTask &cb)
    : _id_(id),
      _delay_time_(delay_time),
      _is_cancel_(false),
      _on_timer_task_(cb)
{
}

TimerTask::~TimerTask()
{
    if (_is_cancel_ == false)
    {
        _on_timer_task_();
    }
    _release_task_();
}

uint32_t TimerTask::getDelayTime()
{
    return _delay_time_;
}

void TimerTask::canceled() // 取消任务
{
    _is_cancel_ = true;
}

void TimerTask::setReleaseTask(const ReleaseTask &cb)
{
    _release_task_ = cb;
}