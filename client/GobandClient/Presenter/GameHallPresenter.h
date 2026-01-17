#ifndef GAMEHALLPRESENTER_H
#define GAMEHALLPRESENTER_H

#include <QObject>

#include "GameHallModel.h"
#include "GameHallView.h"

class GameHallPresenter : public QObject
{
    Q_OBJECT
public:
    explicit GameHallPresenter(GameHallModel* model,GameHallView* view,QObject *parent = nullptr);

signals:

private slots:
    void updateView();//更新view

private:

private:
    GameHallModel* m_model;
    GameHallView* m_view;
};

#endif // GAMEHALLPRESENTER_H
