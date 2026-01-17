#ifndef SESSIONMODEL_H
#define SESSIONMODEL_H

#include <QObject>
#include <functional>
#include "NetWorkManager.h"
#include "Protocol.h"

class SessionModel : public QObject
{
    Q_OBJECT
public:
    explicit SessionModel(QObject *parent = nullptr);
    ~SessionModel() = default;

    void registerMessageHandler(int protocolType, std::function<void(const QJsonObject &)> handler);
    void sendMessage(int protocolType, const QJsonObject& message);

private:
    //消息处理表中所有函数
    void LoginResponse(const QJsonObject& json);

signals:
    void SignalLoginResponse(bool success);//发送响应信号给Login模块

private slots:
    void handlerMessage(const QJsonObject& message);

private:
    QHash<int, std::function<void(const QJsonObject &)>> m_MessageHandlers;//消息处理表
};

#endif // SESSIONMODEL_H
