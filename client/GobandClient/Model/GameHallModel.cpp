#include "GameHallModel.h"

GameHallModel::GameHallModel(QObject *parent) : QObject(parent)
{
    m_data._username = "";
    m_data._score = 0;
    m_data._total_count = 0;
}

GameHallData GameHallModel::getData()
{
    return m_data;
}

void GameHallModel::setData(const GameHallData &data)
{
    m_data._username = data._username;
    m_data._score = data._score;
    m_data._total_count = data._total_count;
}
