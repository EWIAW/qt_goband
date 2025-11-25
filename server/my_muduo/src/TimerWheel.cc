#include "EventLoop.h"

// 创建定时器
static int createTimerFd()
{
    int timerfd = timerfd_create(CLOCK_MONOTONIC, 0); // 创建一个定时器fd，并且使用单调递增时钟，并且fd是阻塞模式
    if (timerfd < 0)
    {
        LOG_FATAL("timerfd create failed!!!");
    }
    struct itimerspec itime;
    itime.it_value.tv_nsec = 0; // 设置第一次超时的时间
    itime.it_value.tv_sec = 1;
    itime.it_interval.tv_nsec = 0; // 设置第一次超时后，每隔多长时间超时
    itime.it_interval.tv_sec = 1;
    int ret = timerfd_settime(timerfd, 0, &itime, NULL);
    if (ret < 0)
    {
        LOG_FATAL("timerfd_settime failed!!!");
    }
    return timerfd;
}

TimerWheel::TimerWheel(EventLoop *loop)
    : _timer_fd_(createTimerFd()),
      _tick_(0),
      _capacity_(capacity),
      _wheel_(_capacity_),
      _loop_(loop),
      _timer_fd_channel_(new Channel(loop, _timer_fd_))
{
    _timer_fd_channel_->SetReadCallback(std::bind(&TimerWheel::OnTime, this));
    _timer_fd_channel_->EnableReading();
}

TimerWheel::~TimerWheel()
{
}

void TimerWheel::TimerTaskAdd(uint64_t id, uint32_t delay_time, const OnTimerTask &cb) // 添加一个定时任务
{
    _loop_->runInLoop(std::bind(&TimerWheel::TimerTaskAddInLoop, this, id, delay_time, cb));
}

void TimerWheel::TimerTaskRefresh(uint64_t id) // 刷新定时任务
{
    _loop_->runInLoop(std::bind(&TimerWheel::TimerTaskRefreshInLoop, this, id));
}

void TimerWheel::TimerTaskCancel(uint64_t id) // 取消定时任务
{
    _loop_->runInLoop(std::bind(&TimerWheel::TimerTaskCancelInLoop, this, id));
}

bool TimerWheel::TimerTaskIsExist(uint64_t id) // 判断定时任务还在不在，true代表在
{
    auto it = _weak_ptr_map_.find(id);
    if (it == _weak_ptr_map_.end())
    {
        return false;
    }
    return true;
}

int TimerWheel::GetTimerFd() // 获取定时器的fd
{
    return _timer_fd_;
}

void TimerWheel::RemoveWeakPtrMap(uint64_t id) // 从_weak_ptr_map_中移除任务
{
    _weak_ptr_map_.erase(id);
}

void TimerWheel::WheelRunOne() // 时间轮向后走一步，走一步后，执行该位置的所有定时任务
{
    _tick_ = (_tick_ + 1) % _capacity_;
    _wheel_[_tick_].clear();
}

int TimerWheel::ReadTimerFd() // 从定时器fd中读取超时次数
{
    uint64_t times;
    int ret = read(_timer_fd_, &times, sizeof(times));
    if (ret < 0)
    {
        LOG_FATAL("ReadTImerFd falied!!!");
    }
    return times;
}

void TimerWheel::OnTime() // 定时器超时后，所执行的任务
{
    int times = ReadTimerFd();
    for (int i = 0; i < times; i++)
    {
        WheelRunOne();
    }
}

void TimerWheel::TimerTaskAddInLoop(uint64_t id, uint32_t delay_time, const OnTimerTask &cb) // 添加定时任务 由 EventLoop来完成
{
    SharedTaskPtr task(new TimerTask(id, delay_time, cb));
    task->setReleaseTask(std::bind(&TimerWheel::RemoveWeakPtrMap, this, id));
    _wheel_[(_tick_ + delay_time) % _capacity_].push_back(task);
    _weak_ptr_map_[id] = WeakTaskPtr(task);
}

void TimerWheel::TimerTaskRefreshInLoop(uint64_t id) // 刷新定时任务 由 EventLoop来完成
{
    auto it = _weak_ptr_map_.find(id);
    if (it == _weak_ptr_map_.end())
    {
        return; // 没有找到定时任务，无法刷新
    }
    SharedTaskPtr task = it->second.lock(); // 通过weak_ptr获取shared_ptr
    int delay_time = task->getDelayTime();
    _wheel_[(_tick_ + delay_time) % _capacity_].push_back(task);
}

void TimerWheel::TimerTaskCancelInLoop(uint64_t id) // 取消定时任务 由EventLoop来完成
{
    auto it = _weak_ptr_map_.find(id);
    if (it == _weak_ptr_map_.end())
    {
        return;
    }
    SharedTaskPtr task = it->second.lock();
    if (task)
    {
        task->canceled();
    }
}