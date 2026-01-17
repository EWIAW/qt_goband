#ifndef GAMEHALLMODEL_H
#define GAMEHALLMODEL_H

#include <QObject>

struct GameHallData
{
    QString _username;
    int _score;
    int _total_count;//总对战场数
};

class GameHallModel : public QObject
{
    Q_OBJECT
public:
    explicit GameHallModel(QObject *parent = nullptr);

    GameHallData getData();
    void setData(const GameHallData& data);

signals:
    void dataChange();//数据改变信号

private slots:

private:
    GameHallData m_data;
};

#endif // GAMEHALLMODEL_H
