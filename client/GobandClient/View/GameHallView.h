#ifndef GAMEHALLVIEW_H
#define GAMEHALLVIEW_H

#include <QWidget>

#include "GameHallModel.h"

namespace Ui {
class GameHallView;
}

class GameHallView : public QWidget
{
    Q_OBJECT

public:
    explicit GameHallView(QWidget *parent = nullptr);
    ~GameHallView();

    void displayView(const GameHallData& data);//展示界面

private:


private:
    Ui::GameHallView *ui;
};

#endif // GAMEHALLVIEW_H
