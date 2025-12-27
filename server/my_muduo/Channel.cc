#include <sys/epoll.h>

#include "Channel.h"
#include "EventLoop.h"

// 保存事件可读可写的状态
const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = EPOLLIN | EPOLLPRI; // EPOLLPRI代表有紧急数据要监听
const int Channel::kWriteEvent = EPOLLOUT;

Channel::Channel(EventLoop *loop, int fd)
    : _loop_(loop),
      _fd_(fd),
      _events_(0),
      _revents_(0),
      _index_(-1), //-1代表该channel未添加到poller中
      _tied_(false)
{
}

Channel::~Channel()
{
}

void Channel::tie(const std::shared_ptr<void> &obj) // 在构建TcpConnection的时候，将TcpConnection的shared_ptr给到_tie_，用于回调时使用
{
    _tie_ = obj;
    _tied_ = true;
}

// channel的更新需要调用epoll_ctl，但是epoll_ctl是封装在Poller当中的，所以不能直接去嗲用
// 而Poller又封装在EventLoop中，所以需要去调用EventLoop的update
void Channel::update()
{
    _loop_->updateChannel(this);
}

// 将channel从epoll模型中删除
void Channel::remove()
{
    _loop_->removeChannel(this);
}

// channel得到poller通知后，处理事件
void Channel::HandlerEvent(Timestamp receiveTime) // 在处理事件之前，要先判断是否启动了防御机制，即if(_tied_)判断
{
    if (_tied_)
    {
        std::shared_ptr<void> guard = _tie_.lock(); // 确保在调用回调期间，TcpConncection的生命周期一直都在
        if (guard)
        {
            HandlerEventWithGuard(receiveTime);
        }
    }
    else
    {
        HandlerEventWithGuard(receiveTime);
    }
}

void Channel::HandlerEventWithGuard(Timestamp receiveTime)
{
    if ((_revents_ & EPOLLHUP) && !(_revents_ & EPOLLIN)) // EPOLLHUP说明对端关闭连接，即调用了close
    {                                                     // !(_revents_ & EPOLLIN)说明对端无残留数据，可以直接关闭连接
        if (_CloseCallback_)
            _CloseCallback_();
    }

    if (_revents_ & EPOLLERR)
    {
        if (_ErrorCallback_)
            _ErrorCallback_();
    }

    if (_revents_ & (EPOLLIN | EPOLLPRI))
    {
        if (_ReadCallback_)
            _ReadCallback_(receiveTime);
    }

    if (_revents_ & EPOLLOUT)
    {
        if (_WriteCallback_)
            _WriteCallback_();
    }

    // 发生任意事件后，都要执行的回调，这里其实是去调用定时器的功能，即刷新定时任务
    if (_EventCallback_)
    {
        _EventCallback_();
    }
}
// 在HandlerEvent函数中，我所理解的防御机制就是，开启了 当回调TcpConnection的函数的时候，我要确保这个TcpConnecion的生命周期是要一直存在的
// 这里我之前遗留的问题是：
// muduo库是一个one loop per thread思想的网络库，即一个线程一个循环，每个线程所属一个EventLoop，
// 每个EventLoop都管理着自己的TcpConnecion，按道理来说，在执行HandleEvent回调的时候，这个TcpConnction连接是不会被销毁的，
// 为什么要进行一个_tied_的判断呢，我觉得这样会很奇怪，因为在执行回调的时候，按道理来说，是不可能会出现TcpConnection被销毁的。
//
// 我给出的理解是，这样的做法是一个防御性编程，因为按正常来说，上面所说的情况是不会发生的，
// 但是有可能是muduo库的使用者由于编程错误，导致在别的EventLoop中调用了别的EventLoop的shutdown，而这个shutdown又没有添加到该EventLoop的任务队列中
// 导致在别的EventLoop中销毁了TcpConnecion。

// 补充上面所遗留的问题：一个客户端连接有可能会先触发 可读 事件，然后再触发 关闭 事件，
// 所以在服务端这边，可能看到的是，可读事件和关闭事件同时触发，则在调用HandlerEventWithGuard的时候，
// 会先调用closecallback，如果没有启用tie机制，当调用完closecallback后，TcpConnecion就被销毁了，
// 导致调用完closecallback后，还会回来调用readcallback，这个时候就会出现问题，所以要引入tie机制来保护

// 又或者说，当发生回调后，在readcallback里面，用户释放了shared_ptr的最后一个引用计数，
// 然后，readcallback调用完成后，又回到HandlerEventWithGuard函数，但是此时还有别的回调要执行，
// 这个时候就会出现问题。
