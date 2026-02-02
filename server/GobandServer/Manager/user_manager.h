#pragma once

#include <unordered_map>
#include <memory>
#include <mutex>
#include <string>
#include <jsoncpp/json/json.h>

#include "../Protocol/Protocol.h"
#include "user_info.h"
#include "TcpConnection.h"

class UserManager
{
private:
    std::unordered_map<uint64_t, uint64_t> _connToId;   // TcpConnection连接ID -> 用户ID
    std::unordered_map<uint64_t, UserInfo> _usersTable; // 用户ID -> 用户信息
    std::mutex _mutex;

public:
    UserManager() {}

    Json::Value UserRegister(const std::string &username, const std::string &password); // 用户注册
    Json::Value UserLogin(const std::string &username, const std::string &password);    // 用户登录

    bool addUser(const uint64_t &tcpConnId, const uint64_t &userId, const UserInfo &info); // 将用户信息添加到UserManager中
    void removeUser(const uint64_t &tcpConnId);                                            // 将用户信息从UserManager中删除
    bool isOnline(const uint64_t &userId);                                                 // 判断用户是否在线
    bool isInRoom(const uint64_t &userId);                                                 //判断用户是否在房间中
};