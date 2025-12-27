#include "ProtocolRouter.h"
#include <QDebug>

ProtocolRouter::ProtocolRouter(QObject* parent)
    :QObject(parent)
{

}

ProtocolRouter* ProtocolRouter::instance()
{
    static ProtocolRouter instance;
    return &instance;
}

// 注册消息处理器
void ProtocolRouter::registerHandler(int protocolId, MessageHandler *handler)
{
    if(m_handlers.contains(protocolId) == true)
    {
        qWarning()<<"Protocol："<<protocolId<<"："<<"is already registered!";
    }
    qDebug()<<"Protocol："<<protocolId<<"："<<"is registered.";
    m_handlers[protocolId] = handler;
}

// 注销消息处理器
void ProtocolRouter::unregisterHandler(int protocolId, MessageHandler *handler)
{
    if(m_handlers.contains(protocolId) == true && m_handlers[protocolId] == handler)
    {
        qDebug()<<"Protocol："<<protocolId<<"："<<"is unregister.";
        m_handlers.remove(protocolId);
    }
}

// 分发消息
void ProtocolRouter::dispatchMessage(int protocolId, const QByteArray &data)
{
    if(m_handlers.contains(protocolId) == true)
    {
        bool ret = m_handlers[protocolId]->handleMessage(protocolId,data);
        if(ret == false)
        {
            qWarning()<<"HandleMessage failed for Protocol："<<protocolId;
        }
    }
    else
    {
        qWarning()<<"No handler for Protocol："<<protocolId;
    }
}
