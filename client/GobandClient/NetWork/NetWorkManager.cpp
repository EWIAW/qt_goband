#include "NetWorkManager.h"

NetWorkManager::NetWorkManager(QObject *parent)
    : QObject(parent),
      m_TcpSocket(new QTcpSocket(this))
{

}

void NetWorkManager::connectToServer(const QString &ip, const quint16 &port)
{
    m_TcpSocket->connectToHost(ip,port);
}

void NetWorkManager::disconnectFromServer()
{
    m_TcpSocket->disconnectFromHost();
}

bool NetWorkManager::isConnected()
{
    return m_TcpSocket->state() == QAbstractSocket::ConnectedState;
}
