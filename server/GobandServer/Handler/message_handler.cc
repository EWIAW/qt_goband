#include "message_handler.h"
#include "Log.h"

MessageHandler::MessageHandler(UserManager &userManager)
    : _userManager(userManager)
{
}

void MessageHandler::handleLoginRequest(const std::shared_ptr<TcpConnection> &conn, const Json::Value &request)
{
    std::string username = request["username"].asCString();
    std::string password = request["password"].asCString();

    Json::Value response = _userManager.UserLogin(username, password);

    // 如果登录成功，设置用户连接
    if (response["success"].asBool())
    {
        uint64_t userId = response["userId"].asUInt64();

        // 构造用户信息插入到user_manager中
        UserInfo info(userId, username, conn);
        _userManager.setUserInfo(userId, info);
        _userManager.setIdFromConn(conn->getid(), userId);
        _userManager.setIdFromName(username, userId);
    }
    sendJsonMessage(conn, Protocol::LOGIN_RESPONSE, response);
}

// 构造发送消息的json数据格式如下:
//{
//     "protocol":"4"
//     "data":{
//               "success":true
//               "userid":1
//               "username":"zrb"
//            }
// }
void MessageHandler::sendJsonMessage(const std::shared_ptr<TcpConnection> &conn, const int &protocol, const Json::Value &message)
{
    // 构造需要发送的json消息
    Json::Value response;
    response["protocol"] = protocol;
    response["data"] = message;

    // 将json转换成字符串
    Json::FastWriter writer;
    std::string jsonStr = writer.write(response);

    // 添加消息头
    int size = jsonStr.size();
    int nsize = htonl(size); // 网络字节序长度

    // 真正发送的json消息
    std::string msg;
    msg.resize(4 + size);
    memcpy(&msg[0], &nsize, sizeof(nsize));
    memcpy(&msg[4], jsonStr.c_str(), size);

    LOG_DEBUG("Send the message size: %d", size);
    LOG_DEBUG(response.toStyledString().c_str());
    conn->send(msg);
}