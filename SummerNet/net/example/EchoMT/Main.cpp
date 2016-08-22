#include <SummerNet/net/example/EchoMT/EchoMT.h>
#include <SummerNet/net/EventLoop.h>
#include <SummerNet/net/InetAddress.h>

using namespace summer;
using namespace summer::net;
using namespace summer::example;

int main()
{
    EventLoop loop;
    InetAddress listenAddr(8888);

    EchoServerMT server(&loop, listenAddr);
    server.SetThreadNum(8);
    server.Start();
    
    loop.Loop();
}