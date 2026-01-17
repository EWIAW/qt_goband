#include "SessionModel.h"

SessionModel::SessionModel(QObject *parent) : QObject(parent)
{
    connect(NetworkManager::instance(),&NetworkManager::handoutMessage,this,&SessionModel::handlerMessage);
    registerMessageHandler(Protocol::LOGIN_RESPONSE,std::bind(&SessionModel::LoginResponse,this,std::placeholders::_1));
}

void SessionModel::registerMessageHandler(int protocolType, std::function<void (const QJsonObject &)> handler)
{
    m_MessageHandlers[protocolType] = handler;
}

void SessionModel::sendMessage(int protocolType, const QJsonObject &message)
{
    NetworkManager::instance()->sendMessage(protocolType,message);
}

void SessionModel::LoginResponse(const QJsonObject &json)
{
    bool success = json["success"].toBool();
    if(success == false)
    {
        emit SignalLoginResponse(success);
    }
    else
    {
        //如果登录成功了，需要将消息分别发给Login和GameHall模块
        emit SignalLoginResponse(success);
    }
}

void SessionModel::handlerMessage(const QJsonObject &message)
{
    // NetWorkManager传过来的json数据格式如下:
    //{
    //     "protocol":"4"
    //     "data":{
    //               "success":true
    //               "userid":1
    //               "username":"zrb"
    //            }
    // }
    //收到NetWorkManager传来的json数据后，进行解析其协议号，并分发处理函数
    int protocolType = message["protocol"].toInt();
    auto it = m_MessageHandlers.find(protocolType);
    if(it == m_MessageHandlers.end())
    {
        qWarning()<<"No handler registered for protocol type:" << protocolType;
    }
    else
    {
        it.value()(message["data"].toObject());
    }
}
