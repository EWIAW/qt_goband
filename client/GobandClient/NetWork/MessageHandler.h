#ifndef MESSAGEHANDLER_H
#define MESSAGEHANDLER_H

#include <QObject>

//网络消息处理    接口类
//如果一个widget对象需要处理网络消息，则需要继承这个类，并重写handleMessage
class MessageHandler:public QObject
{
    Q_OBJECT
public:
    explicit MessageHandler(QObject *parent = nullptr)
        : QObject(parent)
    {

    }

    virtual ~MessageHandler() = default;

    // 处理消息的纯虚函数
    virtual bool handleMessage(const int protocolId, const QByteArray &data) = 0;
};

#endif // MESSAGEHANDLER_H
