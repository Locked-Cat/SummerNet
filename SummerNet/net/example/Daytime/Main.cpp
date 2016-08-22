#include <SummerNet/net/example/Daytime/Daytime.h>
#include <SummerNet/net/EventLoop.h>
#include <SummerNet/net/InetAddress.h>

using namespace summer;
using namespace summer::net;
using namespace summer::example;

int main()
{
    InetAddress listenAddr(13);
    EventLoop loop;
    DaytimeServer server(&loop, listenAddr);
    server.Start();
    loop.Loop();
}