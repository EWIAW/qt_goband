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
    std::unordered_map<uint64_t, UserInfo> _usersTable;  // 用户ID -> 用户信息
    std::unordered_map<uint64_t, uint64_t> _connToId;    // TcpConnection连接ID -> 用户ID
    std::unordered_map<std::string, uint64_t> _nameToId; // 用户名 -> 用户id
    std::mutex _mutex;
    uint64_t _nextUserId;

public:
    UserManager() : _nextUserId(0) {}

    Json::Value UserRegister(const std::string &username, const std::string &password); // 用户注册
    Json::Value UserLogin(const std::string &username, const std::string &password);    // 用户登录

    //_userTable操作
    bool getUserInfo(const uint64_t &userId, UserInfo &info);       // 获取用户信息
    bool setUserInfo(const uint64_t &userId, const UserInfo &info); // 用户信息 填入 _userTable
    void removeUserInfo(const uint64_t &userId);                    // 用户信息 删除 _userTable

    //_connToUesr操作
    uint64_t getIdFromConn(const uint64_t &tcpConnId);                     // 获取用户id
    bool setIdFromConn(const uint64_t &tcpConnId, const uint64_t &userId); // 设置TcpConnection连接ID -> 用户ID
    void removeIdFromConn(const uint64_t &tcpConnId);                      // 删除TcpConnection连接ID -> 用户ID

    //_nameToUser操作
    uint64_t getIdFromName(const std::string &username);                     // 获取用户id
    bool setIdFromName(const std::string &username, const uint64_t &userId); // 设置username -> 用户ID
    void removeIdFromName(const std::string &username);                      // 删除username -> 用户ID

    // 将用户信息完全从UserManager中删除
    void removeUser(const uint64_t &tcpConnId);
};