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
//     "protocol":"3"
//     "data":{
//               "username":"zrb"
//               "password":"123455"
//            }
// }
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

void NetworkManager::onReadyRead()
{
    m_buffer.append(m_socket->readAll());

    //判断是否有完整的消息
    while (m_buffer.size() >= 4)
    {
        int netLength;//网络字节序
        memcpy(&netLength, m_buffer.constData(), sizeof(int));
        int hostLength = qFromBigEndian(netLength);//主机字节序

        if (hostLength > m_buffer.size() - 4)
        {
            // 数据不完整，等待更多数据
            break;
        }

        // 提取一条完整JSON数据
        QByteArray jsonData = m_buffer.mid(4,hostLength);
        m_buffer.remove(0,4+hostLength);

        qCDebug(networkLog)<<"Recv the message size: "<<hostLength;
        qCDebug(networkLog)<<jsonData;
        // 解析并处理消息
        parseMessage(jsonData);
    }
}

// 收到的json数据格式如下:
//{
//     "protocol":"4"
//     "data":{
//               "success":true
//               "userid":1
//               "username":"zrb"
//            }
// }
//将提取处理的字符串时间构造json数据并交给sessionmodel处理
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
    emit handoutMessage(message);
}
