#include "room_manager.h"
#include "Log.h"

RoomManager::RoomManager(UserManager *user_manager)
    : _next_room_id(1),
      _user_manager(user_manager)
{
    DLOG("房间管理模块创建完毕！！！");
}

RoomManager::~RoomManager()
{
    DLOG("房间管理模块销毁完毕！！！");
}

// 创建房间，当两个用户匹配成功时，为他们创建房间
std::shared_ptr<RoomInfo> RoomManager::create_room(const uint64_t &id1, const uint64_t &id2)
{
    // // DLOG("进入create_room函数");
    // // 在创建房间之前，先判断两个用户是否都还在大厅
    // if (_user_manager->is_in_game_hall(id1) == false)
    // {
    //     DLOG("用户：%lu 不在大厅中，创建房间失败", id1);
    //     return std::shared_ptr<RoomInfo>();
    // }
    // if (_user_manager->is_in_game_hall(id2) == false)
    // {
    //     DLOG("用户：%lu 不在大厅中，创建房间失败", id2);
    //     return std::shared_ptr<RoomInfo>();
    // }

    // // 说明两个用户都在大厅中，为他们创建房间
    // std::unique_lock<std::mutex> lock(_mutex);
    // std::shared_ptr<RoomInfo> rp(new RoomInfo(_next_room_id, _user_manager));
    // rp->add_white_user(id1);
    // rp->add_black_user(id2);
    // _rooms.insert(std::make_pair(_next_room_id, rp));
    // _users.insert(std::make_pair(id1, _next_room_id));
    // _users.insert(std::make_pair(id2, _next_room_id));
    // _next_room_id++;
    // // DLOG("创建房间成功");
    // return rp;
}

// 通过房间id获取房间信息
std::shared_ptr<RoomInfo> RoomManager::get_room_by_room_id(const uint64_t &room_id)
{
    // std::unique_lock<std::mutex> lock(_mutex);
    // auto it = _rooms.find(room_id);
    // if (it == _rooms.end())
    // {
    //     return room_ptr();
    // }
    // return it->second;
}

// 通过用户id获取房间信息
std::shared_ptr<RoomInfo> RoomManager::get_room_by_user_id(const uint64_t &user_id)
{
    // std::unique_lock<std::mutex> lock(_mutex);
    // // 先通过用户id找到房间id
    // auto uit = _users.find(user_id);
    // if (uit == _users.end())
    // {
    //     return room_ptr();
    // }
    // auto rit = _rooms.find(uit->second);
    // if (rit == _rooms.end())
    // {
    //     return room_ptr();
    // }
    // return rit->second;
}

// 通过id销毁房间
void RoomManager::remove_room(const uint64_t &room_id)
{
    // 先查找该房间，如果找到了才进行销毁
    std::shared_ptr<RoomInfo> rp = get_room_by_room_id(room_id);
    if (rp == nullptr)
    {
        return;
    }

    std::unique_lock<std::mutex> lock(_mutex);
    uint64_t uid1 = rp->get_white_id();
    uint64_t uid2 = rp->get_black_id();
    _users.erase(uid1);
    _users.erase(uid2);
    _rooms.erase(room_id);
    return;
}

// 删除房间中指定用户，用于用户断开连接时调用
void RoomManager::remove_user(const uint64_t &user_id)
{
    // 先通过用户id查找房间在存不存在
    // room_ptr rp = get_room_by_user_id(user_id);
    // if (rp == nullptr)
    // {
    //     return;
    // }
    // rp->handle_exit(user_id);
    // if (rp->get_player_count() == 0)
    // {
    //     remove_room(rp->get_room_id());
    // }
    // return;
}