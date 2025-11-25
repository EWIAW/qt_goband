#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>

#include "noncopyable.h"
#include "EventLoopThread.h"

class EventLoop;
class EventLoopThread;

// 线程池类，每个线程对应一个EventLoop
class EventLoopThreadPool : noncopyable
{
    using ThreadInitCallback = std::function<void(EventLoop *)>; // 线程初始化函数，如果有

public:
    EventLoopThreadPool(EventLoop *baseloop, const std::string &nameArg);
    ~EventLoopThreadPool();

    void setThreadNum(int numThreads) { _numsThread_ = numThreads; }
    void start(const ThreadInitCallback &cb = ThreadInitCallback());

    // 如果是多线程运行，则使用轮询的方式分配channel给subloop
    EventLoop *getNextLoop();

    std::vector<EventLoop *> getAllLoop();

    bool started() { return _started_; }
    const std::string name() { return _name_; }

private:
    EventLoop *_baseLoop_;
    std::string _name_;
    bool _started_;
    int _numsThread_;
    int _next_; // 用来记录获取下一个subloop
    std::vector<std::unique_ptr<EventLoopThread>> _threads_;
    std::vector<EventLoop *> _loops_;
};