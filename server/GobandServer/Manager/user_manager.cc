#include "user_manager.h"

Json::Value UserManager::UserRegister(const std::string &username, const std::string &password)
{
    std::lock_guard<std::mutex> lock(_mutex);

    Json::Value response;
    response["type"] = Protocol::REGISTER_RESPONSE;

    // 查数据库，检查用户名是否已存在
    // 新用户注册
    // uint64_t userId = _nextUserId++;
    // UserInfo newUser(userId, username, password);
    // _usersTableTableTable[userId] = newUser;
    // _usernameToId[username] = userId;

    // response["success"] = true;
    // response["userId"] = userId;
    // response["username"] = username;

    return response;
}

Json::Value UserManager::UserLogin(const std::string &username, const std::string &password)
{
    std::lock_guard<std::mutex> lock(_mutex);

    Json::Value response;
    response["type"] = Protocol::LOGIN_RESPONSE;

    // 查数据库
    //  检查用户名是否已存在
    //  auto userIt = _nameToUser.find(username);
    //  if (userIt != _nameToUser.end())
    //  {
    //      // 用户名已存在，检查密码
    //      uint64_t userId = userIt->second;
    //      auto userInfoIt = _usersTableTable.find(userId);
    //      if (userInfoIt != _usersTableTable.end() && userInfoIt->second.password == password)
    //      {
    //          // 登录成功
    //          response["success"] = true;
    //          response["userId"] = userId;
    //          response["username"] = username;
    //      }
    //      else
    //      {
    //          // 密码错误
    //          response["success"] = false;
    //          response["error"] = "Invalid password";
    //      }
    //  }

    if (username == "zrb" && password == "123456")
    {
        // 登录成功
        response["success"] = true;
        response["userId"] = 1;
        response["username"] = username;
    }

    if (username == "rjk" && password == "123456")
    {
        // 登录成功
        response["success"] = true;
        response["userId"] = 2;
        response["username"] = username;
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
    std::lock_guard<std::mutex> lock(_mutex);

    uint64_t userId = getIdFromConn(tcpConnId);
    UserInfo user_info;
    getUserInfo(userId, user_info);
    std::string username = user_info._userName;

    removeUserInfo(userId);
    removeIdFromConn(tcpConnId);
    removeIdFromName(username);
}

uint64_t UserManager::getUserRoomId(const uint64_t &userId)
{
    std::lock_guard<std::mutex> lock(_mutex);
    auto it = _usersTable.find(userId);
    if (it != _usersTable.end())
    {
        return it->second._roomId;
    }
    return 0;
}

bool UserManager::setUserRoomId(const uint64_t &userId, const uint64_t &roomId)
{
    std::lock_guard<std::mutex> lock(_mutex);
    auto it = _usersTable.find(userId);
    if (it != _usersTable.end())
    {
        it->second._roomId = roomId;
        return true;
    }
    return false;
}

// void UserManager::removeUserConnection(uint64_t connId)
// {
//     std::lock_guard<std::mutex> lock(_mutex);
//     auto it = _connToUser.find(connId);
//     if (it != _connToUser.end())
//     {
//         uint64_t userId = it->second;
//         auto userIt = _usersTable.find(userId);
//         if (userIt != _usersTable.end())
//         {
//             userIt->second._roomId = 0; // 离开房间
//         }
//         _connToUser.erase(it);
//     }
// }

// std::shared_ptr<TcpConnection> UserManager::getUserConnection(uint64_t userId)
// {
//     std::lock_guard<std::mutex> lock(_mutex);
//     auto it = _usersTable.find(userId);
//     if (it != _usersTable.end())
//     {
//         return it->second._uConn.lock(); // 使用lock()获取shared_ptr
//     }
//     return nullptr;
// }

// uint64_t UserManager::getUserId(const std::string &username)
// {
//     std::lock_guard<std::mutex> lock(_mutex);
//     auto it = _nameToUser.find(username);
//     if (it != _nameToUser.end())
//     {
//         return it->second;
//     }
//     return 0;
// }

// bool UserManager::usernameExists(const std::string &username)
// {
//     std::lock_guard<std::mutex> lock(_mutex);
//     return _nameToUser.find(username) != _nameToUser.end();
// }

// void UserManager::removeUser(uint64_t userId)
// {
//     std::lock_guard<std::mutex> lock(_mutex);
//     auto userIt = _usersTable.find(userId);
//     if (userIt != _usersTable.end())
//     {
//         std::string username = userIt->second._userName;
//         _usersTable.erase(userIt);
//         _nameToUser.erase(username);
//     }
// }