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

signals:
    void loginRequest(const QString& username, const QString& password);

private slots:
    void onLoginButtonClicked();

private:
    Ui::LoginView *ui;
};

#endif // LOGINVIEW_H
