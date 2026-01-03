#include "GobandServer.h"

int main()
{
    EventLoop loop;
    InetAddress addr(3489, "0.0.0.0");
    GobandServer server(&loop, addr, "GobandServer"); // Acceptor non-blocking listenfd  create bind
    server.start();                                   // listen  loopthread  listenfd => acceptChannel => mainLoop =>
    loop.loop();                                      // 启动mainLoop的底层Poller

    return 0;
}