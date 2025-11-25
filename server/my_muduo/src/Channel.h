#pragma once

#include <functional>
#include <memory>
#include <sys/syscall.h>
#include <unistd.h>

#include "noncopyable.h"
#include "Timestamp.h"
#include "Logger.h"

class EventLoop;

// Channel为通道，用于封装sockfd和epoll需要监听的事件，还绑定了poller返回的具体事件
// 当channel底层的fd有事件发生的时候，就会调用相应的回调，其中这些回调方法都是TcpConnection设置给channel的
// 也就是说，当底层有事件发生的时候，去调用的回调方法，本质上是调用TcpConnection的成员函数
class Channel : noncopyable
{
    using EventCallback = std::function<void()>;
    using ReadEventCallback = std::function<void(Timestamp)>;

public:
    Channel(EventLoop *loop, int fd);
    ~Channel();

    // fd得到poller通知后，处理事件
    void HandlerEvent(Timestamp receiveTime);

    // 设置回调函数对象
    void SetReadCallback(ReadEventCallback cb) { _ReadCallback_ = std::move(cb); }
    void SetWriteCallback(EventCallback cb) { _WriteCallback_ = std::move(cb); }
    void SetCloseCallback(EventCallback cb) { _CloseCallback_ = std::move(cb); }
    void SetErrorCallback(EventCallback cb) { _ErrorCallback_ = std::move(cb); }
    void SetEventCallback(EventCallback cb) { _EventCallback_ = std::move(cb); }

    // 防止channel被remove掉后，channel还在执行回调
    void tie(const std::shared_ptr<void> &); // ？？？

    int fd() const { return _fd_; }
    int events() { return _events_; }                     // 返回fd所注册的监听事件
    void SetRevents(int revents) { _revents_ = revents; } // 设置fd所就绪的事件

    // 设置fd相应的事件状态，这里的update本质上是调用EventLoop来实现的，
    // 因为update是改变Poller类对象的信息，而channel和Poller是两个独立类，无法访问对方的成员函数
    // 所以需要去调用EventLoop的update成员函数
    void EnableReading()
    {
        _events_ |= kReadEvent;
        update();
    }
    void DisableReading()
    {
        _events_ &= ~kReadEvent;
        update();
    }
    void EnableWriting()
    {
        _events_ |= kWriteEvent;
        update();
    }
    void DisableWriting()
    {
        _events_ &= ~kWriteEvent;
        update();
    }
    void DisableAll()
    {
        _events_ = kNoneEvent;
        update();
    }

    // 判断fd当前所关心的事件
    bool IsNoneEvent() const { return _events_ == kNoneEvent; }
    bool IsReading() { return _events_ & kReadEvent; }
    bool IsWriting() { return _events_ & kWriteEvent; }

    int index() { return _index_; }                // 判断当前channel这个连接的状态
    void Set_Index(int index) { _index_ = index; } // 设置channel当前连接的状态

    EventLoop *ownerloop() { return _loop_; } // 返回这个channel所属的EventLoop
    void remove();                            // 将channel从epoll模型中删除

private:
    void update(); // 更新channel，例如更新所监听的事件等
    void HandlerEventWithGuard(Timestamp receiveTime);

private:
    // 保存channel所关心的事件
    static const int kNoneEvent;  // 代表该channel没有监听的事件
    static const int kReadEvent;  // 代表该channel监听读事件
    static const int kWriteEvent; // 代表该channel监听写事件

    EventLoop *_loop_; // 这个channel所属那个eventloop循环
    const int _fd_;
    int _events_;  // 注册fd所监听的事件
    int _revents_; // 返回fd就绪的事件
    int _index_;   // 用于帮助poller高效管理channel，例如index为-1，代表channel未加入到epoll模型中

    std::weak_ptr<void> _tie_; // 用于延长TcpConnection的生命周期，确保在TcpConnection回调期间，TcpConnection一直都存在
    bool _tied_;               // 用于辅助_tie_的使用，标记是否启用tie保护机制

    // 当事件发生时，需要调用的回调
    ReadEventCallback _ReadCallback_;
    EventCallback _WriteCallback_;
    EventCallback _CloseCallback_;
    EventCallback _ErrorCallback_;
    EventCallback _EventCallback_; // 触发任何事件后的回调，用于操作定时任务，延长定时任务的销毁时间
};
//_tie_之所以要使用weak_ptr，是因为要防止循环引用的问题，如果使用shared_ptr，则会出现循环引用的问题
// 循环引用链：TcpConnection -> unique_ptr -> Channel -> shared_ptr -> TcpConnection 导致循环引用