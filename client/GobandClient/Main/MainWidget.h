#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QObject>
#include <QWidget>
#include <QStackedWidget>

#include "LoginModel.h"
#include "LoginView.h"
#include "LoginPresenter.h"
#include "GameHallModel.h"
#include "GameHallView.h"
#include "GameHallPresenter.h"

namespace Ui {
class MainWidget;
}

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MainWidget(QWidget *parent = nullptr);
    ~MainWidget();

    void initAllModelAndView();//创建所有model和view
    void initAllPresenter();//创建所有presenter
    void setupConnections();//连接所有页面跳转

private:
    Ui::MainWidget *ui;

    QStackedWidget* m_stackWidget;

    //SessionModel
    SessionModel* m_sessionModle;

    //数据model
    LoginModel* m_loginModel;
    GameHallModel* m_gameHallModel;

    //视图view
    LoginView* m_loginView;
    GameHallView* m_gameHallView;

    //控制器presenter
    LoginPresenter* m_loginPresenter;
    GameHallPresenter* m_gameHallPresenter;
};

#endif // MAINWIDGET_H
