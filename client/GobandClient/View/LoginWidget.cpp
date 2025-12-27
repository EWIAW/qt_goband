#include "LoginWidget.h"
#include "ui_LoginWidget.h"
#include "NetWorkManager.h"

#include <QToolTip>

static QString ip = "175.178.15.192";
static quint16 port = 3489;

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

    sendLoginRequest(user,pwd);
}

void LoginWidget::sendLoginRequest(const QString &user, const QString &pwd)
{
    //先连接到服务器
    NetworkManager::instance()->connectToServer(ip,port);

    //构造json数据
    QJsonObject data;
    data["protocol"] = Protocol::LOGIN_REQUEST;
    data["user"] = user;
    data["pwd"] = pwd;

    QJsonDocument msg(data);
}
