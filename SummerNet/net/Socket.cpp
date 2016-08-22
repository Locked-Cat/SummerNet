#include <SummerNet/net/Socket.h>
#include <SummerNet/net/InetAddress.h>
#include <SummerNet/net/SocketOps.h>
#include <SummerNet/base/Logging.h>

#include <sys/socket.h>
#include <netinet/tcp.h>
#include <strings.h>

using namespace summer;
using namespace summer::net;

Socket::~Socket()
{
    sockets::Close(sockFd_);
}

void Socket::Bind(const InetAddress & addr)
{
    sockets::Bind(sockFd_, addr.GetSockAddr());
}

void Socket::Listen()
{
    sockets::Listen(sockFd_);
}

int Socket::Accept(InetAddress & peerAddr)
{
    struct sockaddr_in addr;
    ::bzero(&addr,sizeof(addr));
    auto connFd = sockets::Accept(sockFd_, &addr);
    if(connFd >= 0)
    {
        peerAddr.SetSockAddr(addr);
    }
    return connFd;
}

void Socket::ShutdownWrite()
{
    sockets::ShutdownWrite(sockFd_);
}

void Socket::SetTcpNoDelay(bool on)
{
    auto optVal = on? 1: 0;
    auto ret = ::setsockopt(sockFd_, IPPROTO_TCP, TCP_NODELAY, &optVal, static_cast<socklen_t>(sizeof(optVal)));
    if(ret != 0)
        LOG_ERROR << "Socket::SetTcpNoDelay()";
}

void Socket::SetReuseAddr(bool on)
{
    auto optVal = on? 1: 0;
    auto ret = ::setsockopt(sockFd_, SOL_SOCKET, SO_REUSEADDR, &optVal, static_cast<socklen_t>(sizeof(optVal)));
    if(ret != 0)
        LOG_ERROR << "Socket::SetReuseAddr()";
}

void Socket::SetReusePort(bool on)
{
    auto optVal = on? 1: 0;
    auto ret = ::setsockopt(sockFd_, SOL_SOCKET, SO_REUSEPORT, &optVal, static_cast<socklen_t>(sizeof(optVal)));
    if(ret != 0)
    {
        LOG_ERROR << "Socket::SetReusePort()";
    }
}

void Socket::SetKeepAlive(bool on)
{
    auto optVal = on? 1: 0;
    auto ret = ::setsockopt(sockFd_, SOL_SOCKET, SO_KEEPALIVE, &optVal, static_cast<socklen_t>(sizeof(optVal)));
    if(ret != 0)
    {
        LOG_ERROR << "Socket::SetKeepAlive()";
    }
}