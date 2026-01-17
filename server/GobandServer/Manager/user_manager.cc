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
        response["userId"] = 1;
        response["username"] = username;
    }
    else if (username == "rjk" && password == "123456")
    {
        // 登录成功
        response["success"] = true;
        response["userId"] = 2;
        response["username"] = username;
    }
    else
    {
        response["success"] = false;
    }
    return response;
}

bool UserManager::getUserInfo(const uint64_t &userId, UserInfo &info) // 获取用户信息
{
    std::lock_guard<std::mutex> lock(_mutex);
    auto it = _usersTable.find(userId);
    if (it != _usersTable.end())
    {
        info = it->second;
        return true;
    }
    return false;
}

bool UserManager::setUserInfo(const uint64_t &userId, const UserInfo &info) // 用户信息 填入 _userTable
{
    std::lock_guard<std::mutex> lock(_mutex);

    auto it = _usersTable.find(userId);
    if (it != _usersTable.end())
    {
        return false; // 说明用户表中已经存在了
    }
    _usersTable[userId] = info;
    return true;
}

void UserManager::removeUserInfo(const uint64_t &userId) // 用户信息 移除 _userTable
{
    std::lock_guard<std::mutex> lock(_mutex);
    _usersTable.erase(userId);
}

uint64_t UserManager::getIdFromConn(const uint64_t &tcpConnId) // 获取用户id
{
    std::lock_guard<std::mutex> lock(_mutex);
    auto it = _connToId.find(tcpConnId);
    if (it != _connToId.end())
    {
        return it->second;
    }
    return 0;
}

bool UserManager::setIdFromConn(const uint64_t &tcpConnId, const uint64_t &userId) // 设置用户id
{
    std::lock_guard<std::mutex> lock(_mutex);

    auto it = _connToId.find(tcpConnId);
    if (it != _connToId.end())
    {
        return false;
    }
    _connToId[tcpConnId] = userId;
    return true;
}

void UserManager::removeIdFromConn(const uint64_t &tcpConnId) // 删除TcpConnection连接ID -> 用户ID
{
    std::lock_guard<std::mutex> lock(_mutex);
    _connToId.erase(tcpConnId);
}

uint64_t UserManager::getIdFromName(const std::string &username) // 获取用户id
{
    std::lock_guard<std::mutex> lock(_mutex);
    auto it = _nameToId.find(username);
    if (it != _nameToId.end())
    {
        return it->second;
    }
    return 0;
}

bool UserManager::setIdFromName(const std::string &username, const uint64_t &userId) // 设置用户id
{
    std::lock_guard<std::mutex> lock(_mutex);

    auto it = _nameToId.find(username);
    if (it != _nameToId.end())
    {
        return false;
    }
    _nameToId[username] = userId;
    return true;
}

void UserManager::removeIdFromName(const std::string &username) // 删除username -> 用户ID
{
    std::lock_guard<std::mutex> lock(_mutex);

    _nameToId.erase(username);
}

void UserManager::removeUser(const uint64_t &tcpConnId) // 将用户信息完全从 UserManager中删除
{
    //整个函数不需要加锁，因为调用了自身的函数
    uint64_t userId = getIdFromConn(tcpConnId);
    UserInfo user_info;
    getUserInfo(userId, user_info);
    std::string username = user_info._userName;

    removeUserInfo(userId);
    removeIdFromConn(tcpConnId);
    removeIdFromName(username);
}