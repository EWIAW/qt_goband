#include "GameHallView.h"
#include "ui_GameHallView.h"

GameHallView::GameHallView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GameHallView)
{
    ui->setupUi(this);
}

GameHallView::~GameHallView()
{
    delete ui;
}

void GameHallView::displayView(const GameHallData &data)
{
    ui->userinfo_TextBrowser->setText(data._username);
}
