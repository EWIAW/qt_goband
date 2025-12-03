#ifndef ECHOSERVER_H
#define ECHOSERVER_H

#include <QWidget>
#include <QTcpSocket>
#include <QString>

namespace Ui {
class EchoServer;
}

class EchoServer : public QWidget
{
    Q_OBJECT

public:
    explicit EchoServer(QWidget *parent = nullptr);
    ~EchoServer();



private:
    void printLog(const QString& msg);//输入日志信息

private slots:
    void connectToServer();//主动连接服务器
    void disconnectToServer();//主动断开服务器

    void onConnection();//已经连接到服务器
    void onDisconnection();//已经断开服务器

    void sendMessage();//发送信息
    void recvMessage();//接受消息

private:
    Ui::EchoServer *ui;
    QTcpSocket *socket;
};

#endif // ECHOSERVER_H
