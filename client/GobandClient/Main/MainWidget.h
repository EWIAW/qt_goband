#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include "PageId.h"
#include "LoginWidget.h"
#include "RegisterWidget.h"
#include "NetWorkManager.h"

#include <QDebug>
#include <QWidget>
#include <QStackedWidget>
#include <QMap>
#include <QSet>

namespace Ui {
class MainWidget;
}

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MainWidget(QWidget *parent = nullptr);
    ~MainWidget();

private:
    void createPage(PageId pageId);//创建页面
    void switchToPage(PageId pageId);//切换页面

    void initLoginWidgetConnections();//连接登录界面的信号槽



public slots:
    void handleLoginRequest(const QString& username, const QString& password);

private:
    Ui::MainWidget *ui;
    NetWorkManager *m_Network;
    QStackedWidget *m_StackWidget;
    QMap<PageId,QWidget*> m_Pages;//存储页面id 和 页面指针 方便快速查找
};

#endif // MAINWIDGET_H
