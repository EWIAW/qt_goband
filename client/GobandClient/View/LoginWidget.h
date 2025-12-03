#ifndef LOGINWIDGET_H
#define LOGINWIDGET_H

#include <QWidget>

namespace Ui {
class LoginWidget;
}

class LoginWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LoginWidget(QWidget *parent = nullptr);
    ~LoginWidget();

signals:
    void loginResquest(const QString& username, const QString& password);

private slots:
    void onLoginButtonClick();

private:
    Ui::LoginWidget *ui;
};

#endif // LOGINWIDGET_H
