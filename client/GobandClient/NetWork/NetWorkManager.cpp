#include <QDataStream>

#include "NetWorkManager.h"

NetworkManager::NetworkManager(QObject* parent)
    :QObject(parent)
    ,m_tcpSocket(new QTcpSocket(this))
{
    connect(m_tcpSocket,&QTcpSocket::readyRead,this,&NetworkManager::onReadyRead);
}

NetworkManager *NetworkManager::instance()
{
    static NetworkManager instance;
    return &instance;
}

NetworkManager::~NetworkManager()
{

}

void NetworkManager::connectToServer(const QString &host, const quint16 &port)
{
    qDebug()<<"connect to Server："<<host<<"："<<port;
    m_tcpSocket->connectToHost(host,port);
}

void NetworkManager::disconnectToServer()
{
    qDebug()<<"disconnect to Server!";
    m_tcpSocket->disconnectFromHost();
}

void NetworkManager::sendMessage(const QJsonDocument& msg)
{
    //发送消息前，需要获取一下连接状态
    if(m_tcpSocket->state() != QAbstractSocket::ConnectedState)
    {
        qWarning()<<"Cannot send message: not connected to server";
        return;
    }

    QByteArray data = msg.toJson(QJsonDocument::Compact);

    //计算消息包长度
    quint32 len = data.size();

    //构造消息包：长度(4)消息体(N)
    QByteArray packet;
    QDataStream stream(&packet, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::BigEndian);

    stream<<len;
    packet.append(data);

    //发送数据
    qint64 ret = m_tcpSocket->write(packet);
    if(ret != packet.size())
    {
        qWarning()<<"Failed to send compelet packet："<<ret<<"/"<<packet.size();
    }

    qDebug()<<"Send Message Success："<<QString(packet);
}


void NetworkManager::onReadyRead()
{
    m_buffer.append(m_tcpSocket->readAll());

    while(m_buffer.size() >= 8)
    {
        // 解析头部：长度(4)+协议号(4)
        QDataStream stream(m_buffer);
        stream.setByteOrder(QDataStream::BigEndian); // 与发送端一致（大端序）

        int packet_len = 0;//消息头
        int protocol = 0;//协议号
        stream>>packet_len;
        stream>>protocol;

        if(packet_len > (m_buffer.size()-4))
        {
            //说明数据包不完整
            break;
        }
        else
        {
            //说明至少有一个完整的数据包
            m_buffer.remove(0,8);
            QByteArray data = m_buffer.left(packet_len);
            m_buffer.remove(0,packet_len);

            ProtocolRouter::instance()->dispatchMessage(protocol,data);
        }
    }
}

