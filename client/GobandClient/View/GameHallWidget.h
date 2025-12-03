#ifndef GAMEHALLWIDGET_H
#define GAMEHALLWIDGET_H

#include <QWidget>

namespace Ui {
class GameHallWidget;
}

class GameHallWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GameHallWidget(QWidget *parent = nullptr);
    ~GameHallWidget();

private:
    Ui::GameHallWidget *ui;
};

#endif // GAMEHALLWIDGET_H
