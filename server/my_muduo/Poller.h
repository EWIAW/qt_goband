// 为什么要将poller层抽象出来
// 因为要给用户提供两种不同IO多路复用的方法：epoll和poll
// 通过使用抽象类，来实现多态，实现可以使用两种不同的IO多路复用的方法
// 但是这里我只实现了epoll的方式
#pragma once

#include <vector>
#include <unordered_map>

#include "noncopyable.h"
#include "Timestamp.h"

class EventLoop;
class Channel;

// poller作为一个抽象的基类
class Poller : noncopyable
{
public:
    using ChannelLists = std::vector<Channel *>; // 用来存储epoll_wait后所返回的就绪fd

    Poller(EventLoop *ownerloop);
    virtual ~Poller() = default;

    // 给所有IO复用保留统一的接口
    virtual Timestamp poll(int timeoutMs, ChannelLists *activeChannel) = 0; // epoll_wait
    virtual void updateChannel(Channel *channel) = 0;                       // epoll_ctl
    virtual void removeChannel(Channel *Channel) = 0;                       // epoll_ctl

    // 判断channel是否在poller中
    bool hasPoller(Channel *Channel) const;

    // Eventloop可以通过该接口获取默认的IO复用的具体实现，即默认获取epoll方式的IO多路复用
    static Poller *newDefaultPoller(EventLoop *loop);

protected:
    // 用于管理sockfd和channel通道，因为一个poller里面会有很多个channel
    using ChannelMap = std::unordered_map<int, Channel *>;
    ChannelMap _channelmap_;

    // private:
    EventLoop *_ownerloop_; // 一个poller所属一个EventLoop
};