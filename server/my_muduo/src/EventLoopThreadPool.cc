#include "EventLoopThreadPool.h"
#include "EventLoopThread.h"
#include "Logger.h"

EventLoopThreadPool::EventLoopThreadPool(EventLoop *baseloop, const std::string &nameArg)
    : _baseLoop_(baseloop),
      _name_(nameArg),
      _started_(false),
      _numsThread_(0),
      _next_(0)
{
}

EventLoopThreadPool::~EventLoopThreadPool()
{
}

void EventLoopThreadPool::start(const ThreadInitCallback &cb)
{
    LOG_DEBUG("EventLoopThreadPool::start , numsThread : %d", _numsThread_);
    _started_ = true;

    for (int i = 0; i < _numsThread_; i++)
    {
        char buffer[_name_.size() + 32];
        snprintf(buffer, sizeof(buffer) - 1, "%s%d", _name_.c_str(), i);
        EventLoopThread *t = new EventLoopThread(cb, buffer);
        _threads_.push_back(std::unique_ptr<EventLoopThread>(t));
        _loops_.push_back(t->startLoop()); // 主线程去调用subloop从线程的startloop
    }

    if (_numsThread_ == 0 && cb)
    {
        cb(_baseLoop_);
    }
}

// 如果是多线程运行，则使用轮询的方式分配channel给subloop
EventLoop *EventLoopThreadPool::getNextLoop()
{
    EventLoop *loop = _baseLoop_;
    if (!_loops_.empty())
    {
        loop = _loops_[_next_];
        _next_++;
        if (_next_ >= _loops_.size())
        {
            _next_ = 0;
        }
    }
    return loop;
}

std::vector<EventLoop *> EventLoopThreadPool::getAllLoop()
{
    if (_loops_.empty())
    {
        return std::vector<EventLoop *>(1, _baseLoop_);
    }
    else
    {
        _loops_;
    }
}