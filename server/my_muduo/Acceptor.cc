#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#include <string.h>

#include "Acceptor.h"
#include "Logger.h"
#include "InetAddress.h"

// 创建一个非阻塞的listenfd
static int createNonblocking()
{
    int sockfd = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0);
    if (sockfd < 0)
    {
        LOG_FATAL("listen socket create error , errno : %d , reason : %s", errno, strerror(errno));
    }
    return sockfd;
}

Acceptor::Acceptor(EventLoop *loop, const InetAddress &listenAddr, bool reuseport)
    : _loop_(loop),
      _acceptSocket_(createNonblocking()), // 创建listenfd
      _acceptChannel_(loop, _acceptSocket_.fd()),
      _listenning_(false)
{
    _acceptSocket_.setReuseAddr(true);      // 设置是否重用地址
    _acceptSocket_.setReusePort(reuseport); // 设置是否重用端口
    _acceptSocket_.bindAddress(listenAddr); // bind
    // TcpServer::start() Acceptor.listen  有新用户的连接，要执行一个回调（connfd=》channel=》subloop）
    // baseLoop => acceptChannel_(listenfd) =>
    _acceptChannel_.SetReadCallback(std::bind(&Acceptor::handlerRead, this));
}

Acceptor::~Acceptor()
{
    _acceptChannel_.DisableAll();
    _acceptChannel_.remove();
}

void Acceptor::listen()
{
    _listenning_ = true;
    _acceptSocket_.listen();
    _acceptChannel_.EnableReading();
}

void Acceptor::handlerRead()
{
    InetAddress peerAddr; // 对端的地址和端口
    int connfd = _acceptSocket_.accept(&peerAddr);
    if (connfd >= 0)
    {
        if (_newConnectionCallback_)
        {
            _newConnectionCallback_(connfd, peerAddr);
        }
        else
        {
            ::close(connfd);
        }
    }
    else
    {
        LOG_ERROR("accept error , errno : %d , reason : %s", errno, strerror(errno));
        if (errno == EMFILE)
        {
            LOG_ERROR("sockfd reached limit"); // 说明可以建立的连接已达上限
        }
    }
}