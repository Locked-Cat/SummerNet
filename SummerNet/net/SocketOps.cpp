#include <SummerNet/net/SocketOps.h>
#include <SummerNet/base/Logging.h>
#include <SummerNet/net/Endian.h>

#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>
#include <cstring>
#include <cstdio>
#include <cassert>
#include <string.h>
#include <sys/uio.h>

using namespace summer::net;

namespace
{
    void SetNonblockAndCloseOnExec(int sockFd)
    {
        auto flags = ::fcntl(sockFd, F_GETFL, 0);
        flags |= O_NONBLOCK;
        auto ret = ::fcntl(sockFd, F_SETFL, flags);

        if(ret < 0)
        {
            LOG_FATAL << "SetNonblockAndCloseOnExec()";
        }

        flags = ::fcntl(sockFd, F_GETFD, 0);
        flags |= FD_CLOEXEC;
        ret = ::fcntl(sockFd, F_SETFD, flags);

        if(ret < 0)
        {
            LOG_FATAL << "SetNonblockAndCloseOnExec()";
        }
    }
}

int sockets::CreateNonblockingSocket()
{
    auto sockFd = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(sockFd < 0)
    {
        LOG_FATAL << "sockets::CreateNonblockingSocket()";
    }

    SetNonblockAndCloseOnExec(sockFd);
    return sockFd;
}

void sockets::Bind(int sockFd, const struct sockaddr_in * addr)
{
    auto ret = ::bind(sockFd, reinterpret_cast<const sockaddr *>(addr), static_cast<socklen_t>(sizeof(sockaddr_in)));
    if(ret < 0)
    {
        char buf[64] = "";
        LOG_FATAL << "sockets::Bind(): " << (strerror_r(errno, buf, sizeof(buf)), buf);
    }
}

void sockets::Listen(int sockFd)
{
    auto ret = ::listen(sockFd, SOMAXCONN);
    if(ret < 0)
    {
        LOG_FATAL << "sockets::Listen()";
    }
}

int sockets::Accept(int sockFd, struct sockaddr_in * addr)
{
    auto addrLen = static_cast<socklen_t>(sizeof(*addr));
    auto connFd = ::accept(sockFd, reinterpret_cast<sockaddr *>(addr), &addrLen);
    SetNonblockAndCloseOnExec(connFd);

    if(connFd < 0)
    {
        auto savedError = errno;
        switch (savedError)
        {
        case EAGAIN:
        case ECONNABORTED:
        case EINTR:
        case EPROTO:
        case EPERM:
        case EMFILE:
            LOG_WARNING << "sockets::Accept()";
            break;
        default:
            LOG_FATAL << "sockets::Accept()";
        }
    }

    return connFd;
}

int sockets::Connect(int sockFd, const struct sockaddr_in * addr)
{
    return ::connect(sockFd, reinterpret_cast<const sockaddr *>(addr), static_cast<socklen_t>(sizeof(sockaddr_in)));
}

ssize_t sockets::Read(int sockFd, void * buf, size_t count)
{
    return ::read(sockFd, buf, count);
}

ssize_t sockets::ReadV(int sockFd, const struct iovec * iov, int iovCnt)
{
    return ::readv(sockFd, iov, iovCnt);
}

ssize_t sockets::Write(int sockFd, const void * buf, size_t count)
{
    return ::write(sockFd, buf, count);
}

void sockets::Close(int sockFd)
{
    if(::close(sockFd))
    {
        LOG_ERROR << "sockets::Close()";
    }
}

void sockets::ShutdownWrite(int sockFd)
{
    if(::shutdown(sockFd, SHUT_WR) < 0)
    {
        LOG_ERROR << "sockets::ShutdownWrite()";
    }
}

void sockets::FromIpPort(const char * ip, uint16_t port, struct sockaddr_in * addr)
{
    addr->sin_family = AF_INET;
    addr->sin_port = HostToNetwork16(port);
    if(::inet_pton(AF_INET, ip, &addr->sin_addr) <=0)
    {
        LOG_ERROR << "sockets::FromIpPort()";
    }
}

void sockets::ToIp(char * buf, size_t size, const struct sockaddr_in * addr)
{
    assert(size >= INET_ADDRSTRLEN);
    auto addr4 = reinterpret_cast<const struct sockaddr_in *>(addr);
    ::inet_ntop(AF_INET, &addr4->sin_addr, buf, static_cast<socklen_t>(size));
}

void sockets::ToIpPort(char * buf, size_t size, const struct sockaddr_in * addr)
{
    ToIp(buf, size, addr);
    auto end = ::strlen(buf);
    auto addr4 = reinterpret_cast<const struct sockaddr_in *>(addr);
    auto port = sockets::NetworkToHost16(addr4->sin_port);
    assert(size > end + sizeof(port));
    snprintf(buf + end, size - end, ":%u", port);
}

int sockets::GetError(int sockFd)
{
    int optVal;
    auto len = static_cast<socklen_t>(sizeof(optVal));

    if(::getsockopt(sockFd, SOL_SOCKET, SO_ERROR, &optVal, &len) < 0)
    {
        return errno;
    }
    else
    {
        return optVal;
    }
}

struct sockaddr_in sockets::GetLocalAddr(int sockFd)
{
    struct sockaddr_in localAddr;
    ::bzero(&localAddr, sizeof(localAddr));
    auto addrLen = static_cast<socklen_t>(sizeof(localAddr));

    if(::getsockname(sockFd, reinterpret_cast<struct sockaddr *>(&localAddr), &addrLen) < 0)
    {
        LOG_ERROR << "sockets::GetLocalAddr()";
    }
    return localAddr;
}

struct sockaddr_in sockets::GetPeerAddr(int sockFd)
{
    struct sockaddr_in peerAddr;
    ::bzero(&peerAddr, sizeof(peerAddr));
    auto addrLen = static_cast<socklen_t>(sizeof(peerAddr));

    if(::getpeername(sockFd, reinterpret_cast<struct sockaddr *>(&peerAddr), &addrLen) < 0)
    {
        LOG_ERROR << "sockets::GetPeerAddr()";
    }
    return peerAddr;
}





