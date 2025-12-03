#include "MainWidget.h"
#include "ui_MainWidget.h"

MainWidget::MainWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWidget),
    m_Network(new NetWorkManager(this)),
    m_StackWidget(new QStackedWidget(this))
{
    ui->setupUi(this);

    m_StackWidget->move(0,0);
    m_StackWidget->resize(1280,720);

    //初始化所有页面，默认使用懒加载
    for(int i = 0; i < VPageid.size();i ++)
    {
        m_Pages[VPageid[i]] = nullptr;
    }

    //默认显示登录界面
    switchToPage(PageId::Login);
}

MainWidget::~MainWidget()
{
    delete ui;
}

void MainWidget::createPage(PageId pageId)
{
    if (m_Pages[pageId] != nullptr)
        return; // 说明已创建

    QWidget* page = nullptr;
    switch (pageId)
    {
    case PageId::Login:
    {
        auto *w = new LoginWidget(this);
        initLoginWidgetConnections();
        page = w;
        break;
    }
    case PageId::Register:
        page = new RegisterWidget(this);
        break;
    case PageId::Count:
        page = nullptr;
        break;
    }

    if (page)
    {
        m_Pages[pageId] = page;
        m_StackWidget->addWidget(page); // 添加到 stackedWidget
    }
}

void MainWidget::switchToPage(PageId pageId)
{
    // 懒加载：只在需要时创建
    if (!m_Pages.contains(pageId))
    {
        qWarning() << "Unknown page:" << static_cast<int>(pageId);
        return;
    }

    createPage(pageId); // 如果未创建，现在创建
    m_StackWidget->setCurrentWidget(m_Pages[pageId]);
}

void MainWidget::initLoginWidgetConnections()
{
    connect(static_cast<LoginWidget*>(m_Pages[PageId::Login]),&LoginWidget::loginResquest,this,&MainWidget::handleLoginRequest);
}

void MainWidget::handleLoginRequest(const QString &username, const QString &password)
{
    //1.连接到服务器
    //2.登录验证
    //3.切换到大厅页面

    //1.
}
