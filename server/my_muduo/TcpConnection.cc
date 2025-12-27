#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "TcpConnection.h"
#include "Logger.h"
#include "Socket.h"
#include "Channel.h"
#include "EventLoop.h"

static EventLoop *CheckLoopNotNull(EventLoop *loop)
{
    if (loop == nullptr)
    {
        LOG_FATAL("loop is null");
    }
    return loop;
}

TcpConnection::TcpConnection(EventLoop *loop,
                             uint64_t id,
                             const std::string &nameArg,
                             int sockfd,
                             const InetAddress &localAddr,
                             const InetAddress &peerAddr)
    : _loop_(CheckLoopNotNull(loop)),
      _id_(id),
      _name_(nameArg),
      _state_(kConnecting),
      _reading_(true),
      _socket_(new Socket(sockfd)),
      _channel_(new Channel(loop, sockfd)),
      _localAddr_(localAddr),
      _peerAddr_(peerAddr),
      _hightWaterMark_(64 * 1024 * 1024) // 64M
{
    _channel_->SetReadCallback(std::bind(&TcpConnection::handleRead, this, std::placeholders::_1));
    _channel_->SetCloseCallback(std::bind(&TcpConnection::handlerClose, this));
    _channel_->SetErrorCallback(std::bind(&TcpConnection::handleError, this));
    _channel_->SetWriteCallback(std::bind(&TcpConnection::handleWrite, this));
    _channel_->SetEventCallback(std::bind(&TcpConnection::handleEvent, this));

    LOG_INFO("TcpConnection is conneceted : name : %s , id : %d , fd : %d", _name_.c_str(), _id_, sockfd);
    _socket_->setKeepAlive(true); // 开启系统自带的保活机制
}

TcpConnection::~TcpConnection()
{
    LOG_INFO("TcpConnection is conneceted : name : %s , id : %d , fd : %d , state : %d",
             _name_.c_str(), _id_, _socket_->fd(), (int)_state_);
}

void TcpConnection::send(const std::string &buf) // 发送数据
{
    if (_state_ == kConnected)
    {
        if (_loop_->isInLoopThread())
        {
            sendInLoop(buf.c_str(), buf.size());
        }
        else
        {
            _loop_->runInLoop(std::bind(&TcpConnection::sendInLoop, this, buf.c_str(), buf.size()));
        }
    }
}

// 在subloop里面发送数据
void TcpConnection::sendInLoop(const void *data, size_t len)
{
    // 发送数据的过程可以分为两种情况：可以直接往sockfd写数据 或 往Buffer里写数据
    ssize_t nwrote = 0;     // 已经写入的数据的长度
    size_t remaining = len; // 未写入的数据的长度
    bool faultError = false;

    if (_state_ == kDisconnected)
    {
        LOG_ERROR("The TcpConnection is disconnected , give up writing！！！");
        return;
    }

    // 1.尝试直接往sockfd写入数据
    if (!_channel_->IsWriting() && _outputBuffer_.readableBytes() == 0)
    {
        nwrote = write(_channel_->fd(), data, len);
        if (nwrote >= 0)
        {
            remaining = len - nwrote;
            // 如果数据已经全部写完了，并且设置了写完后回调
            if (remaining == 0 && _writeCompleteCallback_)
            {
                _loop_->queueInLoop(std::bind(_writeCompleteCallback_, shared_from_this()));
            }
        }
        else
        {
            nwrote = 0;
            if (errno != EWOULDBLOCK) // 如果errno == EWOULDBLOCK，说明底层写入fd已经满了，不能往里面写数据了
            {
                LOG_ERROR("TcpConnection::sendInLoop");
                // EPIPE说明对端关闭了套接字，ECONNRESET说明对端重置了连接
                if (errno == EPIPE || errno == ECONNRESET)
                {
                    faultError = true;
                }
            }
        }
    }

    // 2.往buffer里面写入数据，并注册EPOLLOUT事件
    if (!faultError && remaining > 0)
    {
        // 先判断是否越过水位线
        size_t oldlen = _outputBuffer_.readableBytes();
        if (oldlen + remaining >= _hightWaterMark_ && oldlen < _hightWaterMark_ && _hightWaterMarkCallback_)
        {
            _loop_->queueInLoop(std::bind(_hightWaterMarkCallback_, shared_from_this(), oldlen + remaining));
        }

        _outputBuffer_.append((char *)data + nwrote, remaining);
        if (!_channel_->IsWriting())
        {
            _channel_->EnableWriting();
        }
    }
}

// 关闭连接
void TcpConnection::shutdown() // 这个函数底层调用的是::shutdown(fd,SHUT_WR)，即告诉对端，我不在发送给消息了，是一种半关闭的操作
{
    // LOG_DEBUG("shutdown");
    // LOG_DEBUG("%d", (int)_state_);
    if (_state_ == kConnected)
    {
        // LOG_DEBUG("shutdown in if");
        setState(kDisconnecting);
        _loop_->runInLoop(std::bind(&TcpConnection::shutdownInLoop, this));
    }
}

void TcpConnection::shutdownInLoop()
{
    if (!_channel_->IsWriting()) // 说明ouputbuffer中的数据全部写完
    {
        _socket_->shutdownWrite(); // 关闭写端
    }
}

// 建立连接？？？
void TcpConnection::connectEstablished()
{
    setState(kConnected);
    _channel_->tie(shared_from_this()); // 开启tie机制，确保回调期间，TcpConnecion的生命周期一直都存在
    _channel_->EnableReading();
    _connectionCallback_(shared_from_this()); // 连接完成后，调用连接状态变更回调
}

// 连接销毁
void TcpConnection::connectDestroyed()
{
    if (_state_ == kConnected)
    {
        setState(kDisconnected);
        _channel_->DisableAll();
        _connectionCallback_(shared_from_this());
    }
    if (_loop_->TimerExist(_id_) == true) // 取消关闭该TcpConnection的定时任务
    {
        cancelInactiveReleaseInLoop();
    }
    _channel_->remove();
}

void TcpConnection::handleRead(Timestamp receiveTime)
{
    int savedErrno = 0;
    ssize_t n = _inputBuffer_.readfd(_channel_->fd(), &savedErrno);
    if (n > 0)
    {
        _messageCallback_(shared_from_this(), &_inputBuffer_, receiveTime);
    }
    else if (n == 0)
    {
        handlerClose();
    }
    else
    {
        errno = savedErrno;
        LOG_ERROR("TcpConnection::handleRead");
        handleError();
    }
}

void TcpConnection::handleWrite()
{
    if (_channel_->IsWriting())
    {
        int saveErrno = 0;
        ssize_t n = _outputBuffer_.writefd(_channel_->fd(), &saveErrno);
        if (n > 0)
        {
            _outputBuffer_.retrieve(n);
            if (_outputBuffer_.readableBytes() == 0)
            {
                _channel_->DisableWriting();
                if (_writeCompleteCallback_)
                {
                    _loop_->queueInLoop(std::bind(_writeCompleteCallback_, shared_from_this()));
                }
                if (_state_ == kDisconnecting)
                {
                    shutdownInLoop();
                }
            }
        }
        else
        {
            LOG_ERROR("TcpConnection::handleWrite");
        }
    }
    else
    {
        LOG_ERROR("TcpConnection fd = %d is down , no more writing", _channel_->fd());
    }
}

void TcpConnection::handlerClose()
{
    LOG_INFO("TcpConnection handleClose fd = %d , state = %d", _channel_->fd(), (int)_state_);
    setState(kDisconnected);
    _channel_->DisableAll();

    TcpConnectionPtr connPtr(shared_from_this());
    _connectionCallback_(connPtr); // 执行连接状态变更回调
    _closeCallback_(connPtr);
}

void TcpConnection::handleError()
{
    int err = 0;
    int optval;
    socklen_t optlen = sizeof(optval);
    int n = getsockopt(_channel_->fd(), SOL_SOCKET, SO_ERROR, &optval, &optlen);
    if (n < 0)
    {
        err = errno;
    }
    else
    {
        err = optval;
    }
    LOG_ERROR("TcpConnection::handleError name : %s - SO_ERROR : %d", _name_.c_str(), err);
}

void TcpConnection::handleEvent() // 如果触发任意事件，则刷新定时任务
{
    if (_enable_inactive_release_ == true)
    {
        _loop_->TimerRefresh(_id_);
    }
}

void TcpConnection::enableInactiveReleaseInLoop(int timeouts) // 开启非活跃连接超时机制
{
    _enable_inactive_release_ = true;
    if (_loop_->TimerExist(_id_) == true)
    {
        return _loop_->TimerRefresh(_id_);
    }
    _loop_->TimerAdd(_id_, timeouts, std::bind(&TcpConnection::shutdown, this));
}

void TcpConnection::cancelInactiveReleaseInLoop() // 关闭非活跃连接超时机制
{
    _enable_inactive_release_ = false;
    if (_loop_->TimerExist(_id_) == true)
    {
        _loop_->TimerCancel(_id_);
    }
}

// 当调用shutdown函数后，会发生什么
// 调用shutdown后本质是向对端发送FIN报文，关闭写连接，告诉对端，我不再给你发送数据了，但是还可以接收对端的数据
// 是一种半关闭的状态，当对端真正调用close的时候，就会触发handleclose事件，彻底关闭连接，并销毁释放TcpConecion