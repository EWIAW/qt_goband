#include "EchoServer.h"
#include "MainWidget.h"

#include <QApplication>

//测试回显客户端用
//int main(int argc, char *argv[])
//{
//    QApplication a(argc, argv);
//    EchoServer echoserver;
//    echoserver.show();
//    return a.exec();
//}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    MainWidget mainWindow;
//    mainWindow.show();

    LoginWidget loginWidget;
    loginWidget.show();
    return a.exec();
}
