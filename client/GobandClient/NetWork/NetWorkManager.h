#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QObject>
#include <QTcpSocket>

class NetWorkManager : public QObject
{
    Q_OBJECT
public:
    explicit NetWorkManager(QObject *parent = nullptr);
    ~NetWorkManager() = default;

    //连接/断开
    void connectToServer(const QString &ip, const quint16 &port);
    void disconnectFromServer();
    bool isConnected();

    void sendLoginRequest(const QString& username, const QString& password);
signals:

private slots:
    void onConnected();
    void onDisconnected();
    void onReadyRead();

private:
    void sendToServer(const QJsonObject& obj);//向服务器发送数据函数


private:
    QTcpSocket* m_TcpSocket;
};

#endif // NETWORKMANAGER_H
