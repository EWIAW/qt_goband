#include <functional>

#include "LoginPresenter.h"

LoginPresenter::LoginPresenter(LoginModel* model,LoginView* view, QObject *parent)
    : QObject(parent),
      m_model(model),
      m_view(view)
{
    connect(m_view,&LoginView::loginRequest,this,&LoginPresenter::handleLoginResquest);

    connect(m_model,&LoginModel::SignalLoginResponse,this,&LoginPresenter::handleLoginResponse);
}

void LoginPresenter::handleLoginResquest(const QString &username, const QString &password)
{
    //判断用户名和密码是否位空
    if(username.isNull() || password.isNull())
    {
        m_view->showError("请输入用户名或密码");
        return;
    }
    m_model->onLoginResquest(username,password);
}

void LoginPresenter::handleLoginResponse(bool success)
{
    if(success)
    {
        m_view->showError("登录成功");
    }
    else
    {
        m_view->showError("用户名或密码错误");
    }
}

