#pragma once
#include <string>
#include <memory>

#include "../my_muduo/TcpServer.h"

// 玩家所处的位置
enum Position
{
    GameHall = 0,
    GameRoom = 1
};

// 用户信息
struct UserInfo
{
    Position _pos;                       // 玩家所处的位置
    uint64_t _userId;                    // 用户id，存数据库中
    uint64_t _roomId;                    // 当前所在房间ID
    std::string _userName;               // 用户名
    std::weak_ptr<TcpConnection> _uConn; // 使用weak_ptr避免循环引用

    UserInfo()
        : _pos(Position::GameHall),
          _userId(0),
          _roomId(0),
          _userName(),
          _uConn()
    {
    }

    UserInfo(const uint64_t &uId, const std::string &uName, std::weak_ptr<TcpConnection> uConn)
        : _pos(Position::GameHall),
          _userId(uId),
          _roomId(0),
          _userName(uName),
          _uConn(uConn)
    {
    }
};
