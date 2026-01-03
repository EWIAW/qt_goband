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

    if(username.isEmpty() || password.isEmpty())
    {
        QMessageBox::warning(this, "提示", "请输入用户名和密码");
    }

    emit loginRequest(username, password);
}
