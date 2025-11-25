#include "EventLoopThread.h"
#include "EventLoop.h"

EventLoopThread::EventLoopThread(const ThreadInitCallback &cb,
                                 const std::string &name)
    : _loop_(nullptr),
      _exiting_(false),
      _thread_(std::bind(&EventLoopThread::threadFunc, this), name),
      _mutex_(),
      _cond_(),
      _callback_(cb)
{
}

EventLoopThread::~EventLoopThread()
{
    _exiting_ = true;
    if (_loop_ != nullptr)
    {
        _loop_->quit();
        _thread_.join();
    }
}

// 如果创建了多线程的subloop，则主线程就会运行下面这个函数
EventLoop *EventLoopThread::startLoop()
{
    _thread_.start();
    EventLoop *loop = nullptr;
    {
        std::unique_lock<std::mutex> lock(_mutex_); // unique_lock和Lock_Guard一样，在构造时就会加锁
        while (_loop_ == nullptr)
        {
            _cond_.wait(lock);
        }
        loop = _loop_;
    }
    return loop;
}

// 创建新线程后，新线程会运行下面这个函数
void EventLoopThread::threadFunc()
{
    EventLoop loop;
    if (_callback_)
    {
        _callback_(&loop);
    }
    {
        std::unique_lock<std::mutex> lock(_mutex_);
        _loop_ = &loop;
        _cond_.notify_one();
    }
    loop.loop();
    std::unique_lock<std::mutex> lock(_mutex_);
    _loop_ = nullptr;
}