#include <SummerNet/net/example/Echo/Echo.h>
#include <SummerNet/net/EventLoop.h>
#include <SummerNet/net/InetAddress.h>

using namespace summer;
using namespace summer::net;
using namespace summer::example;

int main()
{
    EventLoop loop;
    InetAddress listenAddr(7);

    EchoServer server(&loop, listenAddr);
    server.Start();
    
    loop.Loop();
}