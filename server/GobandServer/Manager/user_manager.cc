#include "user_manager.h"

Json::Value UserManager::UserRegister(const std::string &username, const std::string &password)
{
    std::lock_guard<std::mutex> lock(_mutex);

    Json::Value response;
    response["type"] = Protocol::REGISTER_RESPONSE;

    return response;
}

Json::Value UserManager::UserLogin(const std::string &username, const std::string &password)
{
    std::lock_guard<std::mutex> lock(_mutex);

    Json::Value response;

    if (username == "zrb" && password == "123456")
    {
        // 登录成功
        response["success"] = true;
        response["userid"] = 1;
        response["username"] = username;
    }
    else if (username == "rjk" && password == "123456")
    {
        // 登录成功
        response["success"] = true;
        response["userid"] = 2;
        response["username"] = username;
    }
    else
    {
        response["success"] = false;
    }
    return response;
}

// 将用户信息添加到UserManager中
bool UserManager::addUser(const uint64_t &tcpConnId, const uint64_t &userId, const UserInfo &info)
{
    std::lock_guard<std::mutex> lock(_mutex);
    auto it1 = _connToId.find(tcpConnId);
    auto it2 = _usersTable.find(userId);
    if (it1 != _connToId.end() || it2 != _usersTable.end())
    {
        return false; // 说明用户表中已经存在了
    }
    _connToId[tcpConnId] = userId;
    _usersTable[userId] = info;
    return true;
}

void UserManager::removeUser(const uint64_t &tcpConnId) // 将用户信息完全从 UserManager中删除
{
    std::lock_guard<std::mutex> lock(_mutex);
    auto it = _connToId.find(tcpConnId);
    if (it != _connToId.end())
    {
        return; // 说明用户表中不存在
    }

    uint64_t userId = it->second;
    _connToId.erase(tcpConnId);
    _usersTable.erase(userId);
}

bool UserManager::isOnline(const uint64_t &userId) // 判断用户是否在线
{
    std::lock_guard<std::mutex> lock(_mutex);
    auto it = _usersTable.find(userId);
    if (it != _usersTable.end())
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool UserManager::isInRoom(const uint64_t &userId) // 判断用户是否在房间中
{
    std::lock_guard<std::mutex> lock(_mutex);
    auto it = _usersTable.find(userId);
    if( it != _usersTable.end())
    {
        if(it->second._pos == Position::GameRoom)
        {
            return true;
        }
    }
    return false;
}
