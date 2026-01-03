#include <functional>

#include "LoginPresenter.h"

LoginPresenter::LoginPresenter(LoginView* view, QObject *parent)
    : QObject(parent),
      m_view(view)
{
    connect(m_view,&LoginView::loginRequest,this,&LoginPresenter::handleLogin);

    //注册消息处理器
    NetworkManager::instance()->registerMessageHandler(Protocol::LOGIN_RESPONSE,
                                                       [this](const QJsonObject &data)
    { handleLoginResponse(data); });
}

void LoginPresenter::handleLogin(const QString &username, const QString &password)
{
    QJsonObject data;
    data["username"] = username;
    data["password"] = password;
    NetworkManager::instance()->sendMessage(Protocol::LOGIN_REQUEST,data);
}

void LoginPresenter::handleLoginResponse(const QJsonObject &data)
{
    qDebug()<<data;
}

