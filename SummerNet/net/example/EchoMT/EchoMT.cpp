#include <SummerNet/net/example/EchoMT/EchoMT.h>
#include <SummerNet/net/TcpConnection.h>
#include <SummerNet/base/Logging.h>
#include <SummerNet/net/Buffer.h>

#include <functional>

using namespace summer;
using namespace summer::net;
using namespace summer::example;
using namespace std;

EchoServerMT::EchoServerMT(EventLoop * loop, const InetAddress & listenAddr)
    : server_(loop, listenAddr, "EchoServerMT")
{
    server_.SetConnectionCallback(std::bind(&EchoServerMT::OnConnection, this, std::placeholders::_1));
    server_.SetMessageCallback(std::bind(&EchoServerMT::OnMessage, this, std::placeholders::_1, std::placeholders::_2));
}

void EchoServerMT::Start()
{
    server_.Start();
}

void EchoServerMT::OnConnection(const TcpConnectionPtr & connection)
{
    LOG_INFO << "EchoServerMT - " << connection->GetPeerAddress().ToIpPort() << ": " << (connection->IsConnected() ? "connected": "disconnected");
}

void EchoServerMT::OnMessage(const TcpConnectionPtr & connection, Buffer & buffer)
{
    auto message = buffer.RetrieveAllAsString();
    connection->Send(message);
    LOG_INFO << "EchoServerMT - " << message;
}