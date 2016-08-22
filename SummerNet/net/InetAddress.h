#ifndef SUMMER_NET_INET_ADDRESS
#define SUMMER_NET_INET_ADDRESS

#include <string>
#include <netinet/in.h>

namespace summer
{
    namespace net
    {
        class InetAddress
        {
        public:
            explicit InetAddress(uint16_t port = 0, bool loopbackOnly = false);
            InetAddress(const std::string & ip, uint16_t port);
            
            explicit InetAddress(const struct sockaddr_in & addr)
                : addr_(addr)
            {
            }

            sa_family_t GetFamily() const
            {
                return addr_.sin_family;
            }

            const struct sockaddr_in * GetSockAddr() const
            {
                return reinterpret_cast<const struct sockaddr_in *>(&addr_);
            }

            void SetSockAddr(const struct sockaddr_in & addr)
            {
                addr_ = addr;
            }

            std::string ToIp() const;
            std::string ToIpPort() const;

            static bool Resolve(const std::string & hostName, InetAddress & result);
        private:
            struct sockaddr_in addr_;
        };
    }
}

#endif