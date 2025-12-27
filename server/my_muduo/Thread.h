#pragma once

#include <thread>
#include <string>
#include <memory>
#include <functional>
#include <atomic>

#include "noncopyable.h"

class Thread : noncopyable
{
    using ThreadFunc = std::function<void()>;

public:
    explicit Thread(ThreadFunc cb, const std::string &tname = std::string());
    ~Thread();

    void start();
    void join();

    bool started() const { return _start_; }
    pid_t tid() const { return _tid_; }
    const std::string &name() { return _tname_; }

    static int numCreated() { return _numCreated_; }

private:
    void setDefaultName(); // 设置线程默认名字

private:
    bool _start_;
    bool _join_;

    std::shared_ptr<std::thread> _thread_;
    pid_t _tid_;
    std::string _tname_;
    ThreadFunc _func_;                   // 创建线程后，线程要执行的函数
    static std::atomic_int _numCreated_; // 记录创建线程的数量
};