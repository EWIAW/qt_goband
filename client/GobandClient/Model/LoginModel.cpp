#include "LoginModel.h"

LoginModel::LoginModel(SessionModel* sessionModel,QObject *parent)
    : QObject(parent),
      m_SessionModel(sessionModel)
{
    connect(m_SessionModel,&SessionModel::SignalLoginResponse,this,&LoginModel::onLoginResponse);
}

//presenter收到view的信号后，所调用的函数
void LoginModel::onLoginResquest(const QString &username, const QString &password)
{

    //构造json数据并发送出去
    QJsonObject data;
    data["username"] = username;
    data["password"] = password;
    m_SessionModel->sendMessage(Protocol::LOGIN_REQUEST,data);
}

//接收SessionModel发来的信号
void LoginModel::onLoginResponse(bool success)
{
    emit SignalLoginResponse(success);
}
