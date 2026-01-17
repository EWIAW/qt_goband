#include "GobandServer.h"
#include "./Protocol/Protocol.h"
#include "Log.h"

GobandServer::GobandServer(EventLoop *loop, const InetAddress &addr, const std::string &name)
    : _server(loop, addr, name),
      _loop(loop),
      _messageHandler(_userManager)

{
    // 注册回调函数
    _server.setConnectionCallback(std::bind(&GobandServer::onConnection, this, std::placeholders::_1));
    _server.setMessageCallback(std::bind(&GobandServer::onMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

    // 设置合适的loop线程数量 loopthread
    _server.setThreadNum(3);

    // 初始化消息处理器
    initMessageHandlers();
    // server_.setEnableInactiveRelease(10);
}

void GobandServer::initMessageHandlers()
{
    _distributeMessage[Protocol::LOGIN_REQUEST] = std::bind(&MessageHandler::handleLoginRequest, &_messageHandler, std::placeholders::_1, std::placeholders::_2);
}

// 服务开始运行
void GobandServer::start()
{
    _server.start();
}

// 连接建立或者断开的回调
void GobandServer::onConnection(const TcpConnectionPtr &conn)
{
    if (conn->connected())
    {
        LOG_DEBUG("Connection UP : %s", conn->peerAddress().GetIpPort().c_str());
    }
    else
    {
        LOG_DEBUG("Connection DOWN : %s", conn->peerAddress().GetIpPort().c_str());

        // 将用户的信息从user_manager中移除
        _userManager.removeUser(conn->getid());
    }
}

// 可读写事件回调
// 传过来的json数据格式如下:
//{
//    "protocol":"1"
//    "data":{
//              "username":"zrb"
//              "password":"123456"
//           }
//}
void GobandServer::onMessage(const TcpConnectionPtr &conn, Buffer *buf, Timestamp time)
{
    // 先判断是否有一条完整的消息
    int size = 0;                 // 消息的长度
    if (buf->readableBytes() > 4) // 说明可以获取到一条消息的消息头
    {
        memcpy(&size, buf->peek(), sizeof(size)); // 先获取消息头
        size = ntohl(size);                       // 转换字节序

        // 判断是否有恶意数据，后期可以根据消息大小进行调整
        if (size > 1024)
        {
            LOG_WARNING("The message size is too big , size:%d", size);
            return;
        }

        if (size >= buf->readableBytes() - 4) // 说明有一条完整的消息
        {
            buf->retrieve(sizeof(size)); // 去除长度消息头
            std::string msg = buf->retrieveAsString(size);

            // 解析JSON消息
            Json::Value root;
            Json::Reader reader;
            if (!reader.parse(msg, root))
            {
                LOG_WARNING("Failed to parse JSON message: %s", msg.c_str());
                return;
            }

            int msgProtocol = root["protocol"].asInt(); // 获取协议号
            Json::Value data = root["data"];            // 获取真正的数据
            auto handlerIt = _distributeMessage.find(msgProtocol);
            if (handlerIt != _distributeMessage.end())
            {
                LOG_DEBUG("Recv the message size: %d", size);
                LOG_DEBUG(root.toStyledString().c_str());
                handlerIt->second(conn, data);
            }
            else
            {
                LOG_WARNING("Unknown message type: %d", msgProtocol);
            }
        }
    }
}