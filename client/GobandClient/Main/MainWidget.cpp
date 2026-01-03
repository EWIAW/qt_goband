#include "MainWidget.h"
#include "ui_MainWidget.h"

#include <QVBoxLayout>

MainWidget::MainWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWidget),
    m_stackWidget(new QStackedWidget(this))
{
    ui->setupUi(this);
    initAllUi();
    initAllConnection();
    // 连接到服务器
    NetworkManager::instance()->connectToServer("175.178.15.192", 3489);
    m_stackWidget->setCurrentWidget(m_loginView);
}

MainWidget::~MainWidget()
{
    delete ui;
    delete m_stackWidget;
}

void MainWidget::initAllUi()
{
    setWindowTitle("五子棋客户端");
    resize(800, 600);

    // 布局管理器
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(m_stackWidget);
    setLayout(layout);

    // 创建各个视图
    m_loginView = new LoginView();

    // 添加到堆栈部件
    m_stackWidget->addWidget(m_loginView);
}

void MainWidget::initAllConnection()
{
    m_loginPresenter = new LoginPresenter(m_loginView, this);
}
