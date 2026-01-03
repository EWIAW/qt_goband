#include "message_handler.h"
#include "Log.h"

MessageHandler::MessageHandler(UserManager &userManager)
    : _userManager(userManager)
{
}

void MessageHandler::handleLoginRequest(const std::shared_ptr<TcpConnection> &conn, const Json::Value &request)
{
    std::string username = request["username"].asString();
    std::string password = request["password"].asString();

    // Json::Value response = _userManager.UserLogin(username, password);

    // // 如果登录成功，设置用户连接
    // if (response["success"].asBool())
    // {
    //     uint64_t userId = response["userId"].asUInt64();

    //     // 构造用户信息插入到user_manager中
    //     UserInfo info(userId, username, conn);
    //     _userManager.setUserInfo(userId, info);
    //     _userManager.setIdFromConn(conn->getid(), userId);
    //     _userManager.setIdFromName(username, userId);
    // }

    // LOG_DEBUG(response.asCString());
    // sendJsonMessage(conn, response);
}

// void MessageHandler::handleRoomListRequest(const std::shared_ptr<TcpConnection>& conn, const Json::Value& request) {
//     Json::Value response = _roomManager.getRoomList();
//     sendJsonMessage(conn, response);
// }

// void MessageHandler::handleCreateRoomRequest(const std::shared_ptr<TcpConnection>& conn, const Json::Value& request) {
//     std::string roomName = request["roomName"].asString();

//     // 获取用户ID
//     uint64_t connId = conn->getid();
//     uint64_t userId = _userManager.getUserId(connId);

//     Json::Value response = _roomManager.createRoom(roomName, userId);

//     if (response["success"].asBool()) {
//         uint64_t roomId = response["roomId"].asUInt64();
//         _userManager.setUserRoomId(userId, roomId);
//     }

//     sendJsonMessage(conn, response);
// }

// void MessageHandler::handleJoinRoomRequest(const std::shared_ptr<TcpConnection>& conn, const Json::Value& request) {
//     uint64_t roomId = request["roomId"].asUInt64();

//     // 获取用户ID
//     uint64_t connId = conn->getid();
//     uint64_t userId = _userManager.getUserId(connId);

//     Json::Value response = _roomManager.joinRoom(roomId, userId);

//     if (response["success"].asBool()) {
//         _userManager.setUserRoomId(userId, roomId);
//     }

//     sendJsonMessage(conn, response);
// }

// void MessageHandler::handleLeaveRoomRequest(const std::shared_ptr<TcpConnection>& conn, const Json::Value& request) {
//     // 获取用户ID
//     uint64_t connId = conn->getid();
//     uint64_t userId = _userManager.getUserId(connId);

//     Json::Value response = _roomManager.leaveRoom(userId, _userManager);

//     sendJsonMessage(conn, response);
// }

// void MessageHandler::handleGameMoveRequest(const std::shared_ptr<TcpConnection>& conn, const Json::Value& request) {
//     int row = request["row"].asInt();
//     int col = request["col"].asInt();

//     // 获取用户ID
//     uint64_t connId = conn->getid();
//     uint64_t userId = _userManager.getUserId(connId);

//     // 获取用户所在房间
//     uint64_t roomId = _userManager.getUserRoomId(userId);
//     if (roomId == 0) {
//         Json::Value errorResponse;
//         errorResponse["type"] = GAME_MOVE_RESPONSE;
//         errorResponse["success"] = false;
//         errorResponse["error"] = "User not in a room";
//         sendJsonMessage(conn, errorResponse);
//         return;
//     }

//     Json::Value response = _roomManager.makeMove(roomId, row, col, userId);

//     // 向房间内所有玩家发送移动消息
//     if (response["success"].asBool()) {
//         _roomManager.sendMessageToRoom(roomId, response, _userManager);

//         // 如果游戏结束，发送游戏结束消息
//         if (response.isMember("gameOver") && response["gameOver"].asBool()) {
//             Json::Value gameOverMsg;
//             gameOverMsg["type"] = GAME_OVER_RESPONSE;
//             gameOverMsg["winner"] = response["winner"];

//             _roomManager.sendMessageToRoom(roomId, gameOverMsg, _userManager);
//         }
//     } else {
//         // 如果移动失败，只发送给当前玩家
//         sendJsonMessage(conn, response);
//     }
// }

// void MessageHandler::handleChatMessageRequest(const std::shared_ptr<TcpConnection>& conn, const Json::Value& request) {
//     std::string message = request["message"].asString();

//     // 获取用户ID
//     uint64_t connId = conn->getid();
//     uint64_t userId = _userManager.getUserId(connId);

//     // 获取用户名
//     UserInfo userInfo;
//     if (!_userManager.getUserInfo(userId, userInfo)) {
//         return;
//     }

//     // 获取用户所在房间
//     uint64_t roomId = _userManager.getUserRoomId(userId);
//     if (roomId == 0) {
//         return;
//     }

//     // 创建聊天消息响应
//     Json::Value chatMsg;
//     chatMsg["type"] = CHAT_MESSAGE_RESPONSE;
//     chatMsg["sender"] = userInfo.username;
//     chatMsg["message"] = message;
//     chatMsg["timestamp"] = static_cast<Json::Int64>(std::time(nullptr));

//     // 向房间内所有玩家发送聊天消息
//     _roomManager.sendMessageToRoom(roomId, chatMsg, _userManager);
// }

void MessageHandler::sendJsonMessage(const std::shared_ptr<TcpConnection> &conn, const Json::Value &message)
{
    Json::FastWriter writer;
    std::string jsonStr = writer.write(message);
    conn->send(jsonStr);
}