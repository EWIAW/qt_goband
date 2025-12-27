// 将线程和EventLoop绑定
#pragma once
#include <mutex>
#include <condition_variable>
#include <functional>
#include <string>

#include "Thread.h"
#include "noncopyable.h"

class EventLoop;

class EventLoopThread : noncopyable
{
    using ThreadInitCallback = std::function<void(EventLoop *)>;

public:
    EventLoopThread(const ThreadInitCallback &cb = ThreadInitCallback(), const std::string &name = std::string());
    ~EventLoopThread();

    EventLoop *startLoop();

private:
    // 创建线程时，线程需要运行的函数
    void threadFunc();

private:
    EventLoop *_loop_;
    bool _exiting_;
    Thread _thread_;
    std::mutex _mutex_;
    std::condition_variable _cond_; // 主线程去创建从线程去运行时，从线程会创建一个所属它自己的eventloop，而且，主线程需要拿到这个eventloop的地址，但是主从线程之间谁先运行不确定，所以需要条件变量去阻塞主线程，等从线程创建好eventloop后，再去唤醒主线程，这样主线程就可以正常获取eventloop的地址了
    ThreadInitCallback _callback_;  // 线程初始化函数回调
};