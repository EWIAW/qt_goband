#pragma once

#include <unordered_map>
#include <memory>

#include "user_manager.h"
#include "room_info.h"

class RoomManager
{
private:
    uint64_t _next_room_id; // 给房间id进行编排序号
    std::mutex _mutex;      // 互斥锁，用来保证哈希表的线程安全
    UserManager *_user_manager;
    std::unordered_map<uint64_t, std::shared_ptr<RoomInfo>> _rooms; // 用来管理通过房间号来找到房间对象
    std::unordered_map<uint64_t, uint64_t> _users;                  // 用来管理通过用户id找到房间id

public:
    RoomManager(UserManager *user_manager);
    ~RoomManager();

    // 创建房间，当两个用户匹配成功时，为他们创建房间
    std::shared_ptr<RoomInfo> create_room(const uint64_t &id1, const uint64_t &id2);

    // 通过房间id获取房间信息
    std::shared_ptr<RoomInfo> get_room_by_room_id(const uint64_t &room_id);

    // 通过用户id获取房间信息
    std::shared_ptr<RoomInfo> get_room_by_user_id(const uint64_t &user_id);

    // 通过id销毁房间
    void remove_room(const uint64_t &room_id);

    // 删除房间中指定用户，用于用户断开连接时调用
    void remove_user(const uint64_t &user_id);
};