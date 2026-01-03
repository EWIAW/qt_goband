// network_manager.cpp
#include "NetWorkManager.h"
#include <QJsonDocument>
#include <QTimer>
#include <QLoggingCategory>
#include <QtEndian>

Q_LOGGING_CATEGORY(networkLog, "network")

NetworkManager *NetworkManager::s_instance = nullptr;

NetworkManager *NetworkManager::instance()
{
    if (!s_instance)
    {
        s_instance = new NetworkManager();
    }
    return s_instance;
}

NetworkManager::NetworkManager(QObject *parent)
    : QObject(parent), m_socket(new QTcpSocket(this))
{
    connect(m_socket, &QTcpSocket::readyRead, this, &NetworkManager::onReadyRead);
    //    connect(m_socket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error),
    //            this, &NetworkManager::onSocketError);
    //    connect(m_socket, &QTcpSocket::connected, this, &NetworkManager::connected);
    //    connect(m_socket, &QTcpSocket::disconnected, this, &NetworkManager::disconnected);
}

NetworkManager::~NetworkManager()
{
    if (m_socket->state() == QTcpSocket::ConnectedState)
    {
        m_socket->disconnectFromHost();
    }
    if(s_instance != nullptr)
    {
        delete s_instance;
    }
}

void NetworkManager::connectToServer(const QString &host, quint16 port)
{
    m_socket->connectToHost(host, port);
}

void NetworkManager::disconnectFromServer()
{
    if (m_socket->state() == QTcpSocket::ConnectedState)
    {
        m_socket->disconnectFromHost();
    }
}

bool NetworkManager::isConnected() const
{
    return m_socket->state() == QTcpSocket::ConnectedState;
}

//发送数据形式
//{
//  protocol:1
//  data
//}

void NetworkManager::sendMessage(int protocolType, const QJsonObject &data)
{
    if (!isConnected())
    {
        qCWarning(networkLog) << "Not connected to server";
        return;
    }

    //构造json数据，即 协议号+data数据
    QJsonObject jsonData;
    jsonData["protocol"] = protocolType;
    jsonData["data"] = data;

    //将上面的json数据转换成ByteArray
    QJsonDocument doc(jsonData);
    QByteArray jsonByteArray = doc.toJson(QJsonDocument::Compact);

    // 构造消息格式: [数据长度][协议号][JSON数据]
    QByteArray messagePacket;//真正要发送的数据
    qint32 jsonStringLength = jsonByteArray.size();
    qint32 bigEndianLen = qToBigEndian(jsonStringLength);//将长度转换为大端字节序
    messagePacket.append(reinterpret_cast<const char *>(&bigEndianLen), sizeof(quint32));
    messagePacket.append(jsonByteArray);

    qCDebug(networkLog)<<jsonStringLength;
    qCDebug(networkLog)<<doc;
    m_socket->write(messagePacket);
    m_socket->flush();
}

void NetworkManager::registerMessageHandler(int protocolType, std::function<void(const QJsonObject &)> handler)
{
    m_messageHandlers[protocolType] = handler;
}

void NetworkManager::onReadyRead()
{
    m_buffer.append(m_socket->readAll());
    qCDebug(networkLog)<<m_buffer;
    //    processData();
}

//void NetworkManager::onSocketError(QAbstractSocket::SocketError error)
//{
//    Q_UNUSED(error)
//    emit connectionError(m_socket->errorString());
//}

void NetworkManager::processData()
{
    while (m_buffer.size() >= sizeof(int))
    {
        char protocolType = m_buffer[0];
        int dataLength;
        memcpy(&dataLength, m_buffer.constData() + 1, sizeof(int));

        if (m_buffer.size() < sizeof(char) + sizeof(int) + dataLength)
        {
            // 数据不完整，等待更多数据
            break;
        }

        // 提取JSON数据
        QByteArray jsonData = m_buffer.mid(sizeof(char) + sizeof(int), dataLength);
        m_buffer = m_buffer.mid(sizeof(char) + sizeof(int) + dataLength);

        // 解析并处理消息
        parseMessage(jsonData);
    }
}

void NetworkManager::parseMessage(const QByteArray &data)
{
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);

    if (error.error != QJsonParseError::NoError)
    {
        qCWarning(networkLog) << "Failed to parse JSON:" << error.errorString();
        return;
    }

    QJsonObject message = doc.object();
    int protocolType = message["type"].toInt();

    auto handler = m_messageHandlers.find(protocolType);
    if (handler != m_messageHandlers.end())
    {
        handler.value()(message["data"].toObject());
    }
    else
    {
        qCWarning(networkLog) << "No handler registered for protocol type:" << protocolType;
    }
}
