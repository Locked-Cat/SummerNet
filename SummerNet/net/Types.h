#ifndef SUMMER_NET_TYPES
#define SUMMER_NET_TYPES

#include <functional>
#include <chrono>
#include <cstdint>
#include <memory>

namespace summer
{
    typedef std::function<void(void)> TimerCallback;
    typedef std::chrono::system_clock::time_point Timestamp;
    typedef std::chrono::milliseconds Timespan;
    typedef std::chrono::system_clock Clock;

    namespace net
    {
        class Buffer;
        class TcpConnection;

        typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
        typedef std::function<void(const TcpConnectionPtr &)> ConnectionCallback;
        typedef std::function<void(const TcpConnectionPtr &)> CloseCallback;
        typedef std::function<void(const TcpConnectionPtr &)> WriteCompleteCallback;
        typedef std::function<void(const TcpConnectionPtr &, Buffer &)> MessageCallback;
    }
}

#endif