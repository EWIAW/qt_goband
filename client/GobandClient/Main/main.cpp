#include "MainWidget.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWidget mainWindow;
    mainWindow.show();

    return a.exec();
}
