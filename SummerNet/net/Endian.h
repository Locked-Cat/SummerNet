#ifndef SUMMER_NET_ENDIAN
#define SUMMER_NET_ENDIAN

#include <arpa/inet.h>

namespace summer
{
    namespace net
    {
        namespace sockets
        {
            inline uint16_t HostToNetwork16(uint16_t host16)
            {
                return htons(host16);
            }

            inline uint16_t NetworkToHost16(uint16_t net16)
            {
                return ntohs(net16);
            }

            inline uint32_t HostToNetwork32(uint32_t host32)
            {
                return htonl(host32);
            }

            inline uint32_t NetworkToHost32(uint32_t net32)
            {
                return ntohl(net32);
            }

            inline uint64_t HostToNetwork64(uint64_t host64)
            {
                return htonll(host64);
            }

            inline uint64_t NetworkToHost64(uint64_t net64)
            {
                return ntohll(net64);
            }
        }
    }
}

#endif