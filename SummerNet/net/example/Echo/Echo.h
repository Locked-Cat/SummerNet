#ifndef EXAMPLE_ECHO
#define EXAMPLE_ECHO

#include <SummerNet/net/TcpServer.h>
#include <SummerNet/net/EventLoop.h>
#include <SummerNet/net/InetAddress.h>

namespace summer
{
    namespace example
    {
        class EchoServer
        {
        private:
            summer::net::TcpServer server_;
            void OnConnection(const summer::net::TcpConnectionPtr & connection);
            void OnMessage(const summer::net::TcpConnectionPtr & connection, summer::net::Buffer & buffer);
        public:
            EchoServer(summer::net::EventLoop * loop, const summer::net::InetAddress & listenAddr);

            void Start();
        };
    }
}

#endif