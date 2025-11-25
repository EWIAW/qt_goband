#pragma once

#include <unistd.h>

#include <atomic>
#include <memory>
#include <vector>
#include <mutex>
#include <functional>

#include "Timestamp.h"
#include "Poller.h"
#include "CurrentThread.h"
#include "noncopyable.h"
#include "TimerWheel.h"

class Channel;
class Poller;
class TimerWheel;

class EventLoop : noncopyable
{
    using Functor = std::function<void()>;

public:
    EventLoop();
    ~EventLoop();

    void loop(); // 开启事件循环
    void quit(); // 退出事件循环

    // 返回poll(epoll_wait)被调用的时间
    // 用const来修饰，可以明确的知道，这个函数是不会改变对象的成员变量的，同时也更安全，因为const的EventLoop对象也能调用
    Timestamp pollReturnTime() const { return _pollReturnTIme_; }

    // 在当前loop中执行回调
    void runInLoop(Functor cb);
    // 把 回调 放入队列中，唤醒loop所在线程，执行cb
    void queueInLoop(Functor cb);

    // 唤醒loop所在线程
    void wakeup(); // ！！！wakeup的核心思想是，避免subloop阻塞在epoll_wait中，让subloop尽快的从epoll_wait中脱离出来

    // Channel需要update，需要去调用poller的方法，但是这两个是两个独立的类，
    // 之间不能直接访问访问对方的成员函数，所有需要借助eventloop来访问
    void updateChannel(Channel *channel);
    void removeChannel(Channel *channel);
    bool hasChannel(Channel *channel);

    // 判断当前EventLoop是否在自己的线程中
    bool isInLoopThread() { return _threadId_ == CurrentThread::tid(); }

    // 定时器操作
    void TimerAdd(uint64_t id, uint32_t delay_time, const OnTimerTask &cb) { _timer_wheel_.TimerTaskAdd(id, delay_time, cb); }
    void TimerRefresh(uint64_t id) { _timer_wheel_.TimerTaskRefresh(id); }
    void TimerCancel(uint64_t id) { _timer_wheel_.TimerTaskCancel(id); }
    bool TimerExist(uint64_t id) { return _timer_wheel_.TimerTaskIsExist(id); }

    int GetTimerFd() { return _timer_wheel_.GetTimerFd(); } // 获取该EventLoop的定时器的fd

private:
    void handlerRead();      // 用于wakeupFd的读事件就绪回调
    void doPendingFunctor(); // 执行已经存储好的回调

private:
    using ChannelLists = std::vector<Channel *>; // 存储事件就绪的channel

    std::atomic_bool _looping_; // 标识eventloop正在循环
    std::atomic_bool _quit_;    // 标识退出loop循环
    const pid_t _threadId_;     // 标识这个loop属于那个线程

    Timestamp _pollReturnTIme_;       // 记录poll(epoll_wait)函数被调用的时间
    std::unique_ptr<Poller> _poller_; // 一个循环一个线程，一个epoll模型

    // 当mainloop获取一个新用户的channel时，通过轮询算法选择一个subloop，通过该成员变量唤醒subloop处理channel
    // 用于在别的eventloop中唤醒这个eventloop
    int _wakeupFd_;
    std::unique_ptr<Channel> _wakeupChannel_;

    ChannelLists _activeChannels_; // 存储事件就绪的channel

    std::atomic_bool _callingPendingFunctors_; // 判断当前loop是否有需要/正在执行的回调
    std::vector<Functor> _pendingFunctors_;    // 存储loop所有需要执行的回调
    std::mutex _mutex_;

    TimerWheel _timer_wheel_; // 一个EventLoop一个定时器时间轮
};
// 我对_wakeupFd_的理解
// 首先subloop线程一旦创建，就是无止境的loop(epoll_wait)，而且这里epoll_wait的超时时间是10秒
// 如果在这10秒内，没有channel发生事件就绪，
// 然后，此时mainloop有一个新连接过来了，mainloop给这个新连接的fd封装成channel后，需要将它添加到poller中，
// 就需要将这个添加新连接操作函数queueInLoop到EventLoop中，但是此时EventLoop真正loop，而且才刚开始一秒
// 还需要9秒后，epoll_wait才返回，所以这个时候处理新连接的函数就不能被及时运行，导致新连接一直在等
// 这样新连接要9秒后才能连接成功，这样的做法对客户端不友好
// 所以在这段时间内，要通过给_wakeupFd_写入数据，从而触发epoll_wait成功，后subloop去执行所有需要执行的回调

// 上面这只是举一个小例子，如果有任何上面所说的可能，都需要用这种方式去唤醒线程，及时处理回调任务