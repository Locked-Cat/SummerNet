#include <SummerNet/net/InetAddress.h>
#include <SummerNet/base/Logging.h>
#include <SummerNet/net/SocketOps.h>
#include <SummerNet/net/Endian.h>

#include <netdb.h>
#include <strings.h>
#include <netinet/in.h>
#include <cassert>
#include <errno.h>

using namespace summer;
using namespace summer::net;

InetAddress::InetAddress(uint16_t port, bool loopbackOnly)
{
    ::bzero(&addr_, sizeof(addr_));
    addr_.sin_family = AF_INET;
    auto ip = loopbackOnly? INADDR_LOOPBACK: INADDR_ANY;
    addr_.sin_addr.s_addr = sockets::HostToNetwork32(ip);
    addr_.sin_port = sockets::HostToNetwork16(port);
}

InetAddress::InetAddress(const std::string & ip, uint16_t port)
{
    ::bzero(&addr_, sizeof(addr_));
    sockets::FromIpPort(ip.c_str(), port, &addr_);
}

std::string InetAddress::ToIp() const
{
    char buf[64] = "";
    sockets::ToIp(buf, sizeof(buf), GetSockAddr());
    return buf;
}

std::string InetAddress::ToIpPort() const
{
    char buf[64] = "";
    sockets::ToIpPort(buf, sizeof(buf), GetSockAddr());
    return buf;
}

bool InetAddress::Resolve(const std::string & hostName, InetAddress & result)
{
    struct addrinfo * addrInfo = nullptr;
    struct addrinfo hints;
    ::bzero(&hints, sizeof(hints));

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    auto ret = ::getaddrinfo(hostName.c_str(), nullptr, &hints, &addrInfo);
    if(ret == 0)
    {
        assert(addrInfo->ai_family == AF_INET);
        result.addr_ = *reinterpret_cast<struct sockaddr_in *>(addrInfo->ai_addr);
        ::freeaddrinfo(addrInfo);
        return true;
    }
    else
    {
        LOG_ERROR << "InetAddress::Resolve()";
        return false;
    }
}