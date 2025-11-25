#pragma once

#include "InetAddress.h"

class Socket
{
public:
    explicit Socket(const int sockfd)
        : _sockfd_(sockfd)
    {
    }

    ~Socket();

    int fd() const { return _sockfd_; }

    // 以下三个函数给listenfd使用
    void bindAddress(const InetAddress &localaddr);
    void listen();
    int accept(InetAddress *peeraddr);

    void shutdownWrite();

    void setTcpNoDelay(bool on);
    void setReuseAddr(bool on);
    void setReusePort(bool on);
    void setKeepAlive(bool on);

private:
    const int _sockfd_;
};