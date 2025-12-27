#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QTcpSocket>
#include <QByteArray>
#include <QJsonObject>
#include <QJsonDocument>
#include "ProtocolRouter.h"
#include "Protocol.h"

class NetworkManager:public QObject
{
    Q_OBJECT
private:
    QTcpSocket* m_tcpSocket;
    QByteArray m_buffer;

private:
    explicit NetworkManager(QObject* parent = nullptr);

public:
    static NetworkManager* instance();
    ~NetworkManager() override;

    //连接到服务器
    void connectToServer(const QString& host, const quint16& port);

    //断开到服务器
    void disconnectToServer();

    //发送消息
    void sendMessage(const QJsonDocument& msg);

private slots:
    void onReadyRead();
};

#endif
