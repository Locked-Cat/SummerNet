#ifndef EXAMPLE_ECHO_MT
#define EXAMPLE_ECHO_MT

#include <SummerNet/net/TcpServer.h>
#include <SummerNet/net/EventLoop.h>
#include <SummerNet/net/InetAddress.h>

namespace summer
{
    namespace example
    {
        class EchoServerMT
        {
        private:
            summer::net::TcpServer server_;
            void OnConnection(const summer::net::TcpConnectionPtr & connection);
            void OnMessage(const summer::net::TcpConnectionPtr & connection, summer::net::Buffer & buffer);
        public:
            EchoServerMT(summer::net::EventLoop * loop, const summer::net::InetAddress & listenAddr);

            void Start();
            void SetThreadNum(int num)
            {
                server_.SetThreadNum(num);
            }
        };
    }
}

#endif