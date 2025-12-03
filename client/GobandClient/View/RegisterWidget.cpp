#include "RegisterWidget.h"
#include "ui_RegisterWidget.h"

RegisterWidget::RegisterWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RegisterWidget)
{
    ui->setupUi(this);
}

RegisterWidget::~RegisterWidget()
{
    delete ui;
}
