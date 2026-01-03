#pragma once
#include "../my_muduo/TcpServer.h"

#include "./Handler/message_handler.h"
#include "./Manager/user_manager.h"

class GobandServer
{
private:
    EventLoop *_loop;
    TcpServer _server;

    UserManager _userManager;

    MessageHandler _messageHandler;
    std::unordered_map<int, std::function<void(const std::shared_ptr<TcpConnection> &, const Json::Value &)>> _distributeMessage; // 消息分发

public:
    GobandServer(EventLoop *loop, const InetAddress &addr, const std::string &name);
    void start();

private:
    void initMessageHandlers();
    void onConnection(const TcpConnectionPtr &conn);// 连接建立或者断开的回调
    void onMessage(const TcpConnectionPtr &conn,Buffer *buf,Timestamp time);// 可读写事件回调
};