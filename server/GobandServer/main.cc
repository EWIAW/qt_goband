#include "GobandServer.h"
#include "../MysqlConnectionPool/MysqlConnectionPool.h"

int main()
{
    // EventLoop loop;
    // InetAddress addr(3489, "0.0.0.0");
    // GobandServer server(&loop, addr, "GobandServer"); // Acceptor non-blocking listenfd  create bind
    // server.start();                                   // listen  loopthread  listenfd => acceptChannel => mainLoop =>
    // loop.loop();                                      // 启动mainLoop的底层Poller

    MysqlConnectionPool *mcp = MysqlConnectionPool::getConnectionPool();
    std::shared_ptr<MysqlConnection> con = mcp->getMysqlConnection();
    std::string sql = "insert into user(username,password,score,total_count,win_count) values('hzh','123456','1000','0','0')";
    con->update(sql);
    std::cout<<"exit"<<std::endl;
    return 0;
}