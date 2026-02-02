#pragma once

#include <functional>
#include <memory>

#include <jsoncpp/json/json.h>
#include "TcpConnection.h"
#include "../Manager/user_manager.h"

class MessageHandler
{
private:
    UserManager &_userManager;

public:
    MessageHandler(UserManager &userManager);

    // 消息处理函数
    void handleLoginRequest(const std::shared_ptr<TcpConnection> &conn, const Json::Value &request);
    void handleMatchRequest(const std::shared_ptr<TcpConnection> &conn, const Json::Value &request);

private:
    void sendJsonMessage(const std::shared_ptr<TcpConnection> &conn, const int &protocol, const Json::Value &message);
};
