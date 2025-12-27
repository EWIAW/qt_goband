#include <stdlib.h>
#include "Poller.h"
#include "EPollPoller.h"

// 实现获取默认的poller
Poller *Poller::newDefaultPoller(EventLoop *loop)
{
    if (getenv("MUDUO_USE_POLL"))
    {
        return nullptr;
    }
    else
    {
        return new EPollPoller(loop); // 生成epoll实例
    }
}
