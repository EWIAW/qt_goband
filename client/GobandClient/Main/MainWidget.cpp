#include "MainWidget.h"
#include "ui_MainWidget.h"

#include <QVBoxLayout>

MainWidget::MainWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWidget),
    m_stackWidget(new QStackedWidget(this))
{
    ui->setupUi(this);
    initAllModelAndView();
    initAllPresenter();
    setupConnections();
    // 连接到服务器
    NetworkManager::instance()->connectToServer("175.178.15.192", 3489);
    m_stackWidget->setCurrentWidget(m_loginView);//默认显示登录界面
}

MainWidget::~MainWidget()
{
    delete ui;
    delete m_stackWidget;
}

void MainWidget::initAllModelAndView()
{
    setWindowTitle("五子棋客户端");
    resize(800, 600);

    // 布局管理器
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(m_stackWidget);
    setLayout(layout);

    //创建SessionModel
    m_sessionModle = new SessionModel(this);

    //创建各个数据model
    m_loginModel = new LoginModel(m_sessionModle,this);
    m_gameHallModel = new GameHallModel(this);

    // 创建各个视图view
    m_loginView = new LoginView(this);
    m_gameHallView = new GameHallView(this);

    // 添加到堆栈部件
    m_stackWidget->addWidget(m_loginView);
    m_stackWidget->addWidget(m_gameHallView);
}

void MainWidget::initAllPresenter()
{
    m_loginPresenter = new LoginPresenter(m_loginModel,m_loginView, this);
    m_gameHallPresenter = new GameHallPresenter(m_gameHallModel,m_gameHallView,this);
}

void MainWidget::setupConnections()
{
//    connect(m_loginView, &LoginView::jumpToGameHall, [this]() {
//        m_stackWidget->setCurrentWidget(m_gameHallView);
//    });
}
