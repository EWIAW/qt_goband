#pragma once
#include <string>
#include <ctime>

#include <mysql/mysql.h>

// 一个MySQL连接
class MysqlConnection
{
public:
    MysqlConnection();
    ~MysqlConnection();

    // 更新操作insert,delete,updata
    bool update(std::string sql);

    // 查询操作select
    MYSQL_RES *query(std::string sql);

// private:
    // 连接数据库
    bool connect(std::string ip,
                 unsigned short port,
                 std::string username,
                 std::string password,
                 std::string dbname);

    void refreshAliveTime() { _alivetime = clock(); }
    clock_t getAliveeTime() const { return clock() - _alivetime; }

private:
    MYSQL *_conn;
    clock_t _alivetime;
};