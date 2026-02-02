#include "MysqlConnection.h"
#include "MysqlConnectionPoolLog.h"

MysqlConnection::MysqlConnection()
{
    _conn = mysql_init(nullptr);
}

MysqlConnection::~MysqlConnection()
{
    if (_conn != nullptr)
        mysql_close(_conn);
}


//更新操作insert,delete,updata
bool MysqlConnection::update(std::string sql)
{
    if (mysql_query(_conn, sql.c_str()))
    {
        MYSQLLOG("更新失败:" + sql);
        return false;
    }
    return true;
}

//查询操作select
MYSQL_RES *MysqlConnection::query(std::string sql)
{
    if (mysql_query(_conn, sql.c_str()))
    {
        MYSQLLOG("查询失败:" + sql);
        return nullptr;
    }
    return mysql_use_result(_conn);
}

//连接数据库
bool MysqlConnection::connect(std::string ip,
                         unsigned short port,
                         std::string username,
                         std::string password,
                         std::string dbname)
{
    MYSQL *p = mysql_real_connect(_conn, ip.c_str(), username.c_str(),
                                  password.c_str(), dbname.c_str(),
                                  port, nullptr, 0);
    if(p != nullptr)
    {
        return true;
    }
    else
    {
        return false;
    }
}