#include <SummerNet/net/example/Echo/Echo.h>
#include <SummerNet/net/TcpConnection.h>
#include <SummerNet/base/Logging.h>
#include <SummerNet/net/Buffer.h>

#include <functional>

using namespace summer;
using namespace summer::net;
using namespace summer::example;
using namespace std;

EchoServer::EchoServer(EventLoop * loop, const InetAddress & listenAddr)
    : server_(loop, listenAddr, "EchoServer")
{
    server_.SetConnectionCallback(std::bind(&EchoServer::OnConnection, this, std::placeholders::_1));
    server_.SetMessageCallback(std::bind(&EchoServer::OnMessage, this, std::placeholders::_1, std::placeholders::_2));
}

void EchoServer::Start()
{
    server_.Start();
}

void EchoServer::OnConnection(const TcpConnectionPtr & connection)
{
    LOG_INFO << "EchoServer - " << connection->GetPeerAddress().ToIpPort() << ": " << (connection->IsConnected() ? "connected": "disconnected");
}

void EchoServer::OnMessage(const TcpConnectionPtr & connection, Buffer & buffer)
{
    auto message = buffer.RetrieveAllAsString();
    connection->Send(message);
    LOG_INFO << "EchoServer - " << message;
}