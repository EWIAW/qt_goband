#include "GobandServer.h"

GobandServer::GobandServer(EventLoop *loop,
                           const InetAddress &addr,
                           const std::string &name)
    : server_(loop, addr, name),
      loop_(loop)
{
    // 注册回调函数
    server_.setConnectionCallback(
        std::bind(&GobandServer::onConnection, this, std::placeholders::_1));

    server_.setMessageCallback(
        std::bind(&GobandServer::onMessage, this,
                  std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

    // 设置合适的loop线程数量 loopthread
    server_.setThreadNum(1);
    // server_.setEnableInactiveRelease(10);
}

//服务开始运行
void GobandServer::start()
{
    server_.start();
}

// 连接建立或者断开的回调
void GobandServer::onConnection(const TcpConnectionPtr &conn)
{
    if (conn->connected())
    {
        LOG_INFO("Connection UP : %s", conn->peerAddress().GetIpPort().c_str());
    }
    else
    {
        LOG_INFO("Connection DOWN : %s", conn->peerAddress().GetIpPort().c_str());
    }
}

// 可读写事件回调
void GobandServer::onMessage(const TcpConnectionPtr &conn,
                             Buffer *buf,
                             Timestamp time)
{
    std::string msg = buf->retrieveAllAsString();
    conn->send(msg);
}