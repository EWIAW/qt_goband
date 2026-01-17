#include "LoginView.h"
#include "ui_LoginView.h"

#include <QMessageBox>

LoginView::LoginView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LoginView)
{
    ui->setupUi(this);

    connect(ui->login_pushButton,&QPushButton::clicked,this,&LoginView::onLoginButtonClicked);
}

LoginView::~LoginView()
{
    delete ui;
}

void LoginView::onLoginButtonClicked()
{
    QString username = ui->username_lineEdit->text().trimmed();
    QString password = ui->password_lineEdit->text();
    emit loginRequest(username, password);
}

void LoginView::showError(const QString &message)
{
    QMessageBox::warning(this,"提示",message);
}

void LoginView::loginResponse(bool success)
{
    if(success == false)
    {
        showError("用户名或密码错误");
    }
}
