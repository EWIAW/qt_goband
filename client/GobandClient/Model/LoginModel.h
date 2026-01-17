#ifndef LOGINMODEL_H
#define LOGINMODEL_H

#include <QObject>
#include "SessionModel.h"
#include "Protocol.h"

class LoginModel : public QObject
{
    Q_OBJECT
public:
    explicit LoginModel(SessionModel* sessionModel,QObject *parent = nullptr);
    ~LoginModel() = default;

    //presenter收到view的信号后，所调用的函数
    void onLoginResquest(const QString& username,const QString& password);

signals:
    //发送信号给presenter
    void SignalLoginResponse(bool success);

private slots:
    //接收SessionModel发来的信号
    void onLoginResponse(bool success);

private:
    SessionModel* m_SessionModel;
};

#endif // LOGINMODEL_H
