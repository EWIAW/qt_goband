#include "LoginWidget.h"
#include "ui_LoginWidget.h"

#include <QToolTip>

LoginWidget::LoginWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LoginWidget)
{
    ui->setupUi(this);

    connect(ui->loginButton,&QPushButton::clicked,this,&LoginWidget::onLoginButtonClick);
}

LoginWidget::~LoginWidget()
{
    delete ui;
}

void LoginWidget::onLoginButtonClick()
{
    QString user = ui->userNameEdit->text().trimmed();
    QString pwd = ui->passwordEdit->text();

    if(user.isEmpty() || pwd.isEmpty())
    {
        QPoint point = ui->userNameEdit->mapToGlobal(QPoint(0,-(ui->userNameEdit->height())));
        QToolTip::showText(point,"请输入账号和密码",ui->userNameEdit);
        return;
    }

    emit loginResquest(user,pwd);
    return;
}
