#ifndef LOGINVIEW_H
#define LOGINVIEW_H

#include <QWidget>

namespace Ui {
class LoginView;
}

class LoginView : public QWidget
{
    Q_OBJECT

public:
    explicit LoginView(QWidget *parent = nullptr);
    ~LoginView();

    //展示错误信息
    void showError(const QString& message);

    //presenter所调用的函数
    void loginResponse(bool success);

signals:
    //发送信号给presenter
    void loginRequest(const QString& username, const QString& password);

private slots:
    void onLoginButtonClicked();

private:
    Ui::LoginView *ui;
};

#endif // LOGINVIEW_H
