#include "GameHallWidget.h"
#include "ui_GameHallWidget.h"

GameHallWidget::GameHallWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GameHallWidget)
{
    ui->setupUi(this);
}

GameHallWidget::~GameHallWidget()
{
    delete ui;
}
