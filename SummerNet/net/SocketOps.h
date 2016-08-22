#ifndef SUMMER_NET_SOCKETOPS
#define SUMMER_NET_SOCKETOPS

#include <arpa/inet.h>

namespace summer
{
    namespace net
    {
        namespace sockets
        {
            int  CreateNonblockingSocket();
            void Bind(int sockFd, const struct sockaddr_in * addr);
            void Listen(int sockFd);
            int  Accept(int sockFd, struct sockaddr_in * addr);
            int  Connect(int sockFd, const struct sockaddr_in * addr);
            ssize_t Read(int sockFd, void * buf, size_t count);
            ssize_t ReadV(int sockFd, const struct iovec * iov, int iovCnt);
            ssize_t Write(int sockFd, const void * buf, size_t count);
            void Close(int sockFd);
            void ShutdownWrite(int sockFd);

            void FromIpPort(const char * ip, uint16_t port, struct sockaddr_in * addr);
            void ToIp(char * buf, size_t size, const struct sockaddr_in * addr);
            void ToIpPort(char * buf, size_t size, const struct sockaddr_in * addr);

            int GetError(int sockFd);

            struct sockaddr_in GetLocalAddr(int sockFd);
            struct sockaddr_in GetPeerAddr(int sockFd);
        }
    }
}

#endif 