#pragma once
#include "match_queue.h"
#include "user_manager.h"

// 管理匹配队列
class MatchManager
{
private:
    MatchQueue _queue_bronze; // 青铜匹配队列
    UserManager *_user_manager;
    room_manager *_room_manager;

public:
    matcher(online_manager *online_manager, room_manager *room_manager, user_table *user_table)
        : _online_manager(online_manager),
          _room_manager(room_manager),
          _user_table(user_table),
    {
        DLOG("游戏匹配模块处理完毕!!!");
    }

    // 处理匹配功能
    void handler_match(match_queue<uint64_t> &queue)
    {
        while (true)
        {
            // 如果队列中的人数 小于2 则阻塞匹配队列线程
            while (queue.size() < 2)
            {
                queue.wait();
            }
            // DLOG("人数大于2个，开始匹配");
            // DLOG("!!!!!!!!!!");
            // 走到这里说明匹配队列中的人数 大于2
            // 从匹配队列中取出前两个玩家进行创建房间对战
            uint64_t uid1, uid2;
            // DLOG("!!!!!");
            bool ret = queue.pop(uid1);
            // DLOG("pop1");
            if (ret == false) // 即使走到这里，匹配队列中的人数也有可能不足2人，因为如果有人进行匹配后，又取消了就有可能出现这种情况
            {
                // DLOG("1");
                continue;
            }
            ret = queue.pop(uid2);
            // DLOG("pop2");
            if (ret == false)
            {
                // DLOG("2");
                // 走到这里说明有一个人匹配了成功，可对方却取消匹配了，所以要将uid1重新加入匹配队列
                add(uid1);
                continue;
            }
            // 走到这里说明，两人匹配成功，但是匹配成功后，要判断两个人是否都还在游戏大厅
            // DLOG("conn1");
            server_t::connection_ptr conn1 = _online_manager->get_con_from_hall(uid1);
            if (conn1.get() == nullptr) // uid1不在游戏大厅
            {
                // DLOG("3");
                add(uid2);
                continue;
            }
            // DLOG("conn2");
            server_t::connection_ptr conn2 = _online_manager->get_con_from_hall(uid2);
            if (conn2.get() == nullptr) // uid2不在游戏大厅
            {
                // DLOG("4");
                add(uid1);
                continue;
            }
            // 走到这里说明，两个人终于匹配成功，为他们创建房间
            // DLOG("create_room");
            room_ptr rp = _room_manager->create_room(uid1, uid2);
            if (rp.get() == nullptr)
            {
                // DLOG("5");
                add(uid1);
                add(uid2);
                continue;
            }
            // DLOG("!");
            // 对两个玩家进行响应
            Json::Value response;
            response["optype"] = "match_success";
            response["result"] = true;
            std::string body;
            // DLOG("!!!!!!!!!!");
            json_util::serialize(response, body);
            // DLOG("handler_match : %s", body.c_str());
            conn1->send(body);
            conn2->send(body);
        }
        return;
    }

    // 处理青铜匹配队列
    void handler_bronze_match()
    {
        return handler_match(_queue_bronze);
    }

    // 处理白银匹配队列
    void handler_sliver_match()
    {
        return handler_match(_queue_sliver);
    }

    // 处理黄金匹配队列
    void handler_gold_match()
    {
        return handler_match(_queue_gold);
    }

    // 将玩家加入匹配队列
    bool add(const uint64_t &uid)
    {
        // 根据玩家的分数，把不同的玩家加入到不同的匹配队列
        Json::Value message; // 通过用户的id可以将用户的信息传到message中
        bool ret = _user_table->select_by_id(uid, message);
        if (ret == false)
        {
            DLOG("获取用户 %d 信息失败", uid);
            return false;
        }
        uint64_t score = message["score"].asUInt64();
        if (score < 2000)
        {
            _queue_bronze.push(uid);
        }
        else if (score < 3000)
        {
            _queue_sliver.push(uid);
        }
        else
        {
            _queue_gold.push(uid);
        }
        return true;
    }

    // 将玩家从匹配队列中移除，当玩家取消匹配后调用
    bool del(const uint64_t &uid)
    {
        Json::Value message;
        bool ret = _user_table->select_by_id(uid, message);
        if (ret == false)
        {
            DLOG("获取用户 %d 信息失败", uid);
            return false;
        }
        uint64_t score = message["score"].asUInt64();
        if (score < 2000)
        {
            _queue_bronze.remove(uid);
        }
        else if (score < 3000)
        {
            _queue_sliver.remove(uid);
        }
        else
        {
            _queue_gold.remove(uid);
        }
        return true;
    }
};