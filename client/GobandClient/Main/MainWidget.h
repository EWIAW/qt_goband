#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QObject>
#include <QWidget>
#include <QStackedWidget>

#include "LoginView.h"
#include "LoginPresenter.h"

namespace Ui {
class MainWidget;
}

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MainWidget(QWidget *parent = nullptr);
    ~MainWidget();

    void initAllUi();//初始化所有界面
    void initAllConnection();//初始化所有连接

private:
    Ui::MainWidget *ui;

    QStackedWidget* m_stackWidget;

    //视图
    LoginView* m_loginView;

    //控制器
    LoginPresenter* m_loginPresenter;
};

#endif // MAINWIDGET_H
