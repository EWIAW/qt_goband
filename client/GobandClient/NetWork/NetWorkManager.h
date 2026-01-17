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

signals:
    //网络管理器将服务器传来的数据构造完一条完整的json数据后，通过信号发送给sessionmodel
    void handoutMessage(const QJsonObject& message);

private slots:
    void onReadyRead();

private:
    explicit NetworkManager(QObject *parent = nullptr);

    void parseMessage(const QByteArray &data);

    QTcpSocket *m_socket;
    QByteArray m_buffer;
    static NetworkManager *s_instance;
};

#endif
