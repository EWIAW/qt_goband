#ifndef PROTOCOLROUTER_H
#define PROTOCOLROUTER_H

#include <QObject>
#include <QHash>
#include "MessageHandler.h"

//协议路由（单例模式）
//所有需要介绍网络消息的对象，都需要将其 协议号和指针 存储在这个对象中，用于消息路由
class ProtocolRouter:public QObject
{
    Q_OBJECT
private:
    QHash<int,MessageHandler*> m_handlers;//存储协议号与其对应的界面对象

private:
    explicit ProtocolRouter(QObject* parent = nullptr);

public:
    static ProtocolRouter* instance();

    // 注册消息处理器
    void registerHandler(int protocolId, MessageHandler *handler);

    // 注销消息处理器
    void unregisterHandler(int protocolId, MessageHandler *handler);

    // 分发消息
    void dispatchMessage(int protocolId, const QByteArray &data);
};

#endif // PROTOCOLROUTER_H
