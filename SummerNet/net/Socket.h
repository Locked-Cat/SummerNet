#ifndef SUMMER_NET_SOCKET
#define SUMMER_NET_SOCKET

#include <boost/noncopyable.hpp>

namespace summer
{
    namespace net
    {
        class InetAddress;

        class Socket
            : public boost::noncopyable
        {
        public:
            explicit Socket(int sockFd)
                : sockFd_(sockFd)
            {
            }

            ~Socket();

            int GetFd() const
            {
                return sockFd_;
            }

            void Bind(const InetAddress & addr);
            void Listen();
            int  Accept(InetAddress & peerAddr);
            void ShutdownWrite();

            void SetTcpNoDelay(bool on);
            void SetReuseAddr(bool on);
            void SetReusePort(bool on);
            void SetKeepAlive(bool on);
        private:
            const int sockFd_;
        };
    }
}

#endif