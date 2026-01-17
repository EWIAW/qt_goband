#ifndef LOGINPRESENTER_H
#define LOGINPRESENTER_H

#include <QObject>

#include "NetWorkManager.h"
#include "LoginModel.h"
#include "LoginView.h"

class LoginPresenter : public QObject
{
    Q_OBJECT
public:
    explicit LoginPresenter(LoginModel* model, LoginView* view, QObject *parent = nullptr);
    ~LoginPresenter() = default;

signals:

private slots:
    //view发信号，presenter处理
    void handleLoginResquest(const QString& username, const QString& password);

    //model发信号，presenter处理
    void handleLoginResponse(bool success);

private:
    LoginModel* m_model;
    LoginView* m_view;
};

#endif // LOGINPRESENTER_H
