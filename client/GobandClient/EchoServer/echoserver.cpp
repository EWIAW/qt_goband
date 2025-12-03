#include "EchoServer.h"
#include "ui_EchoServer.h"

#include <QMessageBox>
#include <QDateTime>
#include <QDebug>

EchoServer::EchoServer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EchoServer),
    socket(new QTcpSocket(this))
{
    ui->setupUi(this);

    connect(ui->connectButton,&QPushButton::clicked,this,&EchoServer::connectToServer);
    connect(ui->disconnectButton,&QPushButton::clicked,this,&EchoServer::disconnectToServer);

    connect(socket,&QTcpSocket::connected,this,&EchoServer::onConnection);
    connect(socket,&QTcpSocket::disconnected,this,&EchoServer::onDisconnection);

    connect(ui->sendButton,&QPushButton::clicked,this,&EchoServer::sendMessage);
    connect(socket,&QTcpSocket::readyRead,this,&EchoServer::recvMessage);

    connect(ui->sendEdit,&QLineEdit::returnPressed,this,&EchoServer::sendMessage);
}

EchoServer::~EchoServer()
{
    delete ui;
}

void EchoServer::printLog(const QString &msg)
{
    QString time = QDateTime::currentDateTime().toString("hh:mm:ss");
    ui->textView->append(QString("[%1] %2").arg(time, msg));
}

void EchoServer::connectToServer()
{
    //    qDebug()<<"connectToServer";
    socket->connectToHost("175.178.15.192",3489);
    printLog("正在连接中...");
}

void EchoServer::disconnectToServer()
{
    printLog("正在断开连接...");
    socket->disconnectFromHost();
}

void EchoServer::onConnection()
{
    printLog("已经连接到服务器");
}

void EchoServer::onDisconnection()
{
    printLog("已经断开连接服务器");
}

void EchoServer::sendMessage()
{
    QString msg = ui->sendEdit->text().trimmed();
    if(msg.isEmpty())
        return;

    socket->write(msg.toStdString().c_str());
    msg.push_front("Client : ");
    printLog(msg);
    ui->sendEdit->clear();
}

void EchoServer::recvMessage()
{
    QByteArray data = socket->readAll();
    QString msg = QString::fromUtf8(data);
    msg.push_front("Server : ");
    printLog(msg);
}
