#include "GameHallPresenter.h"

GameHallPresenter::GameHallPresenter(GameHallModel* model,GameHallView* view,QObject *parent)
    : QObject(parent),
      m_model(model),
      m_view(view)

{
    connect(m_model,&GameHallModel::dataChange,this,&GameHallPresenter::updateView);
}

void GameHallPresenter::updateView()
{
    m_view->displayView(m_model->getData());
}
