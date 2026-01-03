#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QTcpSocket>
#include <QByteArray>
#include <QJsonObject>
#include <QJsonDocument>
#include "Protocol.h"

class NetworkManager : public QObject
{
    Q_OBJECT

public:
    static NetworkManager *instance();
    ~NetworkManager();

    void connectToServer(const QString &host, quint16 port);
    void disconnectFromServer();
    bool isConnected() const;

    void sendMessage(int protocolType, const QJsonObject &data);

    void registerMessageHandler(int protocolType, std::function<void(const QJsonObject &)> handler);

signals:
//    void connected();
//    void disconnected();
    void connectionError(const QString &error);

private slots:
    void onReadyRead();
//    void onSocketError(QAbstractSocket::SocketError error);

private:
    explicit NetworkManager(QObject *parent = nullptr);

    void processData();
    void parseMessage(const QByteArray &data);

    QTcpSocket *m_socket;
    QByteArray m_buffer;
    static NetworkManager *s_instance;

    QMap<int, std::function<void(const QJsonObject &)>> m_messageHandlers;
};

#endif
