#pragma once

#include <sys/epoll.h>

#include <vector>

#include "Poller.h"
#include "Timestamp.h"

class EventLoop;
class Channel;

// 这个类主要是封装epoll的操作
// 如epoll_create,epoll_ctl,epoll_wait;
class EPollPoller : public Poller
{
    using EventList = std::vector<struct epoll_event>;

public:
    EPollPoller(EventLoop *loop);
    ~EPollPoller() override;

    // 重写基类Poller抽象方法
    Timestamp poll(int timeoutMs, ChannelLists *activeChannel) override;
    void updateChannel(Channel *channel) override;
    void removeChannel(Channel *channel) override;

private:
    // 填写活跃的连接，将底层fd所发生的事件填写到EventLoop的ChannelLists中
    void fillActiveChannel(int numEvents, ChannelLists *activeChannels) const;
    // 更新channel通道
    void update(int operation, Channel *channel);

private:
    // 用于初始化EventList的大小
    static const int kInitEventListSize = 16;
    int _epollfd_;      // epoll_create的返回值 epoll模型的fd
    EventList _events_; // 通过epoll_wait所得到的就绪事件都存储再这个vector中
};
// using EventList = std::vector<struct epoll_event>;
// using ChannelLists = std::vector<Channel *>;

// EventList和ChannelList的区别
// EventList是传参为epoll_wait用的，因为epoll_wait的参数是固定的，即是一个struct epoll_wait的数组
// 但是，当我们获取到返回事件就绪的fd时，希望得到的是，返回就绪fd的channel，
// 因为channel中存储着事件就绪后需要调用的回调函数
// 所以后续步骤中，需要将EventList中的就绪事件 给到 ChannelLists