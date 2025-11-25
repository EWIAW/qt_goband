#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/tcp.h>

#include <string.h>

#include "Socket.h"
#include "Logger.h"

Socket::~Socket()
{
    close(_sockfd_);
}

void Socket::bindAddress(const InetAddress &localaddr)
{
    int ret = bind(_sockfd_, (sockaddr *)localaddr.getSockAddr(), sizeof(sockaddr_in));
    if (ret == -1)
    {
        LOG_FATAL("bind error , errno : %d , reason : %s", errno, strerror(errno));
    }
    LOG_DEBUG("bind listenfd is success");
}

void Socket::listen()
{
    int ret = ::listen(_sockfd_, 1024); // 在muduo库源码中，第二个参数给的是SOMAXCONN，这个值是4096，在以前是128，如果给的值大于SOMAXCONN，则直接截断到该值
    if (ret == -1)
    {
        LOG_FATAL("listen error , errno : %d , reason : %s", errno, strerror(errno));
    }
    LOG_DEBUG("listenfd is listening");
}

// 参数为 输出型参数
int Socket::accept(InetAddress *peeraddr)
{
    sockaddr_in addr;
    socklen_t len = sizeof(addr);
    bzero(&addr, sizeof(addr));
    int connfd = ::accept4(_sockfd_, (sockaddr *)&addr, &len, SOCK_NONBLOCK | SOCK_CLOEXEC);
    if (connfd >= 0)
    {
        peeraddr->setSockAddr(addr);
    }
    return connfd;
}

void Socket::shutdownWrite()
{
    int ret = shutdown(_sockfd_, SHUT_WR);
    if (ret < 0)
    {
        LOG_ERROR("shutdownWrite error : %d , reason : %s", errno, strerror(errno));
    }
}

// 禁用/启用 Nagle 算法。
// Nagle 算法通过合并小数据包减少网络流量，但会增加延迟。
// 设置 TCP_NODELAY 为 1 可以禁用该算法，适用于低延迟要求的应用（如实时通信）。
void Socket::setTcpNoDelay(bool on)
{
    int optval = on ? 1 : 0;
    ::setsockopt(_sockfd_, IPPROTO_TCP, TCP_NODELAY, &optval, sizeof(optval));
}

// 允许快速重新绑定地址（如服务器重启后可立即复用绑定的端口）
void Socket::setReuseAddr(bool on)
{
    int optval = on ? 1 : 0;
    ::setsockopt(_sockfd_, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
}

// 多个进程或线程可以绑定同一端口，常用于高并发服务。？？？
void Socket::setReusePort(bool on)
{
    int optval = on ? 1 : 0;
    ::setsockopt(_sockfd_, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));
}

// 保活机制定期检测连接是否存活，适用于长时间空闲的 TCP 连接。
// 如果on为true，即打开保活机制，若连接长时间未响应，则终止连接
// 但是我使用了自己的时间轮实现了非活跃连接超时断开机制
void Socket::setKeepAlive(bool on)
{
    int optval = on ? 1 : 0;
    ::setsockopt(_sockfd_, SOL_SOCKET, SO_KEEPALIVE, &optval, sizeof(optval));
}