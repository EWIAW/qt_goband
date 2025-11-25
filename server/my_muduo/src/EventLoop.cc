#include <sys/eventfd.h>

#include <string.h>

#include "EventLoop.h"

// 防止一个线程创建多个eventloop thread_local技术
__thread EventLoop *t_loopInThisThread = nullptr;

// 定义epoll_wait所设置的超时时间
const int kPollTimeMs = 10000;

// 创建wakeupfd，用来唤醒subloop来处理新的channel
static int createEventFd() // ！！！
{
    int evtfd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC); // 0代表初始计数器为0
    if (evtfd < 0)
    {
        LOG_DEBUG("create eventfd failed , errno : %d , reason : %s", errno, strerror(errno));
    }
    return evtfd;
}

EventLoop::EventLoop()
    : _looping_(false),
      _quit_(false),
      _threadId_(CurrentThread::tid()),
      // _pollReturnTIme_(0),
      _poller_(Poller::newDefaultPoller(this)),
      _wakeupFd_(createEventFd()),
      _wakeupChannel_(new Channel(this, _wakeupFd_)),
      _callingPendingFunctors_(false),
      _timer_wheel_(this)
{
    LOG_DEBUG("EventLoop create %p in thread %d", this, _threadId_);
    if (t_loopInThisThread != nullptr)
    {
        LOG_FATAL("Another EventLoop %d exists in this thread %d", t_loopInThisThread, _threadId_);
    }
    else
    {
        t_loopInThisThread = this;
    }
    _wakeupChannel_->SetReadCallback(std::bind(&EventLoop::handlerRead, this));
    _wakeupChannel_->EnableReading();
}

EventLoop::~EventLoop()
{
    _wakeupChannel_->DisableAll(); // 从epoll模型删除之前，需要停止监听事件
    _wakeupChannel_->remove();     // 从epoll模型中移除
    close(_wakeupFd_);
    t_loopInThisThread = nullptr;
}

void EventLoop::loop() // 开启事件循环
{
    _looping_ = true;
    _quit_ = false;
    LOG_INFO("EventLoop %p start looping", this);
    while (!_quit_)
    {
        _activeChannels_.clear();
        _pollReturnTIme_ = _poller_->poll(kPollTimeMs, &_activeChannels_);
        for (Channel *channel : _activeChannels_)
        {
            channel->HandlerEvent(_pollReturnTIme_);
        }
        doPendingFunctor();
    }
    LOG_INFO("EventLoop %p stop looping", this);
    _looping_ = false;
}

// 退出事件循环 ！！！
void EventLoop::quit()
{
    _quit_ = true;
    if (!isInLoopThread())
    {
        wakeup();
    }
}

// 需要在该EventLoop中执行回调，如果是在属于自己的EventLoop中，则直接执行回调，否则，将回调加入的任务函数队列中
void EventLoop::runInLoop(Functor cb)
{
    // LOG_DEBUG("EventLoop::runInLoop");
    if (isInLoopThread())
    {
        cb();
    }
    else
    {
        queueInLoop(cb);
    }
}

// 把cb放入回调队列中，唤醒loop所在线程，执行cb
void EventLoop::queueInLoop(Functor cb)
{
    {
        std::unique_lock<std::mutex> lock(_mutex_);
        _pendingFunctors_.emplace_back(cb);
    }

    if (!isInLoopThread() || _callingPendingFunctors_)
    {
        wakeup();
    }
}

// 用于wakeupFd的读事件就绪回调
void EventLoop::handlerRead()
{
    uint64_t one = 1;
    ssize_t n = read(_wakeupFd_, &one, sizeof(one));
    if (n != sizeof(one))
    {
        LOG_ERROR("read the _wakeupFd_ error");
    }
}

// 唤醒一个EventLoop就是往这个EventLoop里面的wakechannel写入数据，这样这个EventLoop的epoll_wait就会立即返回
void EventLoop::wakeup()
{
    uint64_t one = 1;
    ssize_t n = write(_wakeupFd_, &one, sizeof(one));
    if (n != sizeof(one))
    {
        LOG_ERROR("write the _wakeupFd_ error");
    }
}

void EventLoop::updateChannel(Channel *channel)
{
    _poller_->updateChannel(channel);
}

void EventLoop::removeChannel(Channel *channel)
{
    _poller_->removeChannel(channel);
}

bool EventLoop::hasChannel(Channel *channel)
{
    return _poller_->hasPoller(channel);
}

// 这个函数需要加锁，那是因为mainloop有可能正在往subloop的_pendingFunctors_添加回调任务
void EventLoop::doPendingFunctor() // 执行回调
{
    std::vector<Functor> functors; // 使用临时的vector<Functor>那是因为避免mainloop因为等待锁而被阻塞
    _callingPendingFunctors_ = true;

    {
        std::unique_lock<std::mutex> lock(_mutex_);
        functors.swap(_pendingFunctors_);
    }

    for (const Functor &cb : functors)
    {
        cb();
    }
    _callingPendingFunctors_ = false;
}
