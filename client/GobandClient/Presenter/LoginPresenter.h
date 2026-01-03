#ifndef LOGINPRESENTER_H
#define LOGINPRESENTER_H

#include <QObject>

#include "NetWorkManager.h"
#include "LoginView.h"

class LoginPresenter : public QObject
{
    Q_OBJECT
public:
    explicit LoginPresenter(LoginView* view, QObject *parent = nullptr);

signals:

private slots:
    void handleLogin(const QString& username, const QString& password);

private:
    void handleLoginResponse(const QJsonObject& data);

private:
    LoginView* m_view;
};

#endif // LOGINPRESENTER_H
