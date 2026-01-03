#pragma once 

#include <functional>
#include <memory>

#include <jsoncpp/json/json.h>
#include "TcpConnection.h"
#include "../Manager/user_manager.h"

class MessageHandler {
private:
    UserManager& _userManager;
    // RoomManager& _roomManager;

public:
    MessageHandler(UserManager& userManager);
    
    // 消息处理函数
    void handleLoginRequest(const std::shared_ptr<TcpConnection>& conn, const Json::Value& request);
    // void handleRoomListRequest(const std::shared_ptr<TcpConnection>& conn, const Json::Value& request);
    // void handleCreateRoomRequest(const std::shared_ptr<TcpConnection>& conn, const Json::Value& request);
    // void handleJoinRoomRequest(const std::shared_ptr<TcpConnection>& conn, const Json::Value& request);
    // void handleLeaveRoomRequest(const std::shared_ptr<TcpConnection>& conn, const Json::Value& request);
    // void handleGameMoveRequest(const std::shared_ptr<TcpConnection>& conn, const Json::Value& request);
    // void handleChatMessageRequest(const std::shared_ptr<TcpConnection>& conn, const Json::Value& request);
    
private:
    void sendJsonMessage(const std::shared_ptr<TcpConnection>& conn, const Json::Value& message);
};
