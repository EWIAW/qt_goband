#include "room_info.h"
#include "./Log.h"

RoomInfo::RoomInfo(const uint64_t &room_id, UserManager *user_manager)
    : _play_count(0), _room_id(room_id),
      _room_status(GAME_START),
      _user_manager(user_manager),
      _board(BOARD_ROW, std::vector<int>(BOARD_COL, 0))
{
    DLOG("房间创建成功");
}

RoomInfo::~RoomInfo()
{
    DLOG("房间销毁成功");
}

// 获取房间id
uint64_t RoomInfo::get_room_id()
{
    return _room_id;
}

// 获取房间状态
RoomState RoomInfo::get_room_status()
{
    return _room_status;
}

// 获取玩家数量
int RoomInfo::get_player_count()
{
    return _play_count;
}

// 将白方添加进房间
void RoomInfo::add_white_user(const uint64_t &id)
{
    _white_id = id;
    _play_count++;
    return;
}

// 将黑方添加进房间
void RoomInfo::add_black_user(const uint64_t &id)
{
    _black_id = id;
    _play_count++;
    return;
}

// 获取白方用户id
uint64_t RoomInfo::get_white_id()
{
    return _white_id;
}

// 获取黑方用户id
uint64_t RoomInfo::get_black_id()
{
    return _black_id;
}

// 判断某一方向上是否五星连珠
// row和col为下棋的位置，offset为偏移量，依次判断横竖斜四个方向是否五星连珠
bool RoomInfo::five(const int row, const int col, const int offset_row, const int offset_col, const int chess_color)
{
    // DLOG("进入five函数");
    int count = 1;
    int tmp_row = row;
    int tmp_col = col;

    tmp_row += offset_row;
    tmp_col += offset_col;
    while (tmp_row >= 0 && tmp_row < BOARD_ROW && tmp_col >= 0 && tmp_col < BOARD_COL)
    {
        if (_board[tmp_row][tmp_col] == chess_color)
        {
            count++;
            tmp_row += offset_row;
            tmp_col += offset_col;
        }
        else
        {
            break;
        }
    }
    tmp_row = row;
    tmp_col = col;
    tmp_row -= offset_row;
    tmp_col -= offset_col;
    while (tmp_row >= 0 && tmp_row < BOARD_ROW && tmp_col >= 0 && tmp_col < BOARD_COL)
    {
        if (_board[tmp_row][tmp_col] == chess_color)
        {
            count++;
            tmp_row -= offset_row;
            tmp_col -= offset_col;
        }
        else
        {
            break;
        }
    }
    if (count == 5)
    {
        return true;
    }
    else
    {
        return false;
    }
}

// 判断是否获胜，如果获胜了，返回获胜者的id
uint64_t RoomInfo::check_win(const int row, const int col, const int chess_color)
{
    if (five(row, col, 1, 0, chess_color) ||
        five(row, col, 0, 1, chess_color) ||
        five(row, col, -1, -1, chess_color) ||
        five(row, col, -1, 1, chess_color))
    {
        if (chess_color == WHITE_CHESS)
        {
            return _white_id;
        }
        else
        {
            return _black_id;
        }
    }
    return 0;
}