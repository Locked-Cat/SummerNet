#include <SummerNet/net/example/Daytime/Daytime.h>
#include <SummerNet/net/TcpConnection.h>
#include <SummerNet/base/Logging.h>
#include <SummerNet/net/Buffer.h>

#include <functional>
#include <chrono>
#include <ctime>
#include <cstring>

using namespace summer;
using namespace summer::net;
using namespace summer::example;
using namespace std;
using namespace std::chrono;

DaytimeServer::DaytimeServer(EventLoop * loop, const InetAddress & listenAddr)
    : server_(loop, listenAddr, "DaytimeServer")
{
    server_.SetConnectionCallback(std::bind(&DaytimeServer::OnConnection, this, std::placeholders::_1));
    server_.SetMessageCallback(std::bind(&DaytimeServer::OnMessage, this, std::placeholders::_1, std::placeholders::_2));
}

void DaytimeServer::Start()
{
    server_.Start();
}

void DaytimeServer::OnConnection(const TcpConnectionPtr & connection)
{
    LOG_INFO << "DaytimeServer - " << connection->GetPeerAddress().ToIpPort() << ": " << (connection->IsConnected() ? "connected": "disconnected");
    if(connection->IsConnected())
    {
        char buf[64];
        auto now = system_clock::now();
        auto nowTime = system_clock::to_time_t(now);
        ::ctime_r(&nowTime, buf);
        connection->Send(static_cast<const void *>(buf), std::strlen(buf));
        connection->Shutdown();
    }
}

void DaytimeServer::OnMessage(const TcpConnectionPtr & connection, Buffer & buffer)
{
    buffer.RetrieveAll();
    LOG_INFO << "DaytimerServer - Discard all messsage.";
}
