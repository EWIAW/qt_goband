#pragma once

#include <vector>
#include <jsoncpp/json/json.h>

#include "user_manager.h"

#define BOARD_ROW 15  // 棋盘行数
#define BOARD_COL 15  // 棋盘列数
#define WHITE_CHESS 1 // 白方的棋子
#define BLACK_CHESS 2 // 黑方的棋子

// 定义房间状态
enum RoomState
{
    GAME_START = 1,
    GAME_OVER
};

// 房间类，用来维护两个用户匹配成功后，一个小范围的空间
class RoomInfo
{
private:
    int _play_count;                      // 玩家数量
    uint64_t _room_id;                    // 房间id
    uint64_t _white_id;                   // 白方id
    uint64_t _black_id;                   // 黑方id
    RoomState _room_status;               // 房间状态
    UserManager *_user_manager;           // 用户管理类
    std::vector<std::vector<int>> _board; // 棋盘

public:
    RoomInfo(const uint64_t &room_id, UserManager *user_manager);
    ~RoomInfo();

    uint64_t get_room_id();                  // 获取房间id
    RoomState get_room_status();             // 获取房间状态
    int get_player_count();                  // 获取玩家数量
    void add_white_user(const uint64_t &id); // 将白方添加进房间
    void add_black_user(const uint64_t &id); // 将黑方添加进房间
    uint64_t get_white_id();                 // 获取白方用户id
    uint64_t get_black_id();                 // 获取黑方用户id

private:
    // 判断某一方向上是否五星连珠
    // row和col为下棋的位置，offset为偏移量，依次判断横竖斜四个方向是否五星连珠
    bool five(const int row, const int col, const int offset_row, const int offset_col, const int chess_color);

    // 判断是否获胜，如果获胜了，返回获胜者的id
    uint64_t check_win(const int row, const int col, const int chess_color);
};