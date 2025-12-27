#include "../my_muduo/TcpServer.h"

class GobandServer
{
public:
    GobandServer(EventLoop *loop,
                 const InetAddress &addr,
                 const std::string &name);
    void start();

private:
    // 连接建立或者断开的回调
    void onConnection(const TcpConnectionPtr &conn);


    // 可读写事件回调
    void onMessage(const TcpConnectionPtr &conn,
                   Buffer *buf,
                   Timestamp time);


private:
    EventLoop *loop_;
    TcpServer server_;
};