#ifndef SUMMER_NET_ACCEPTOR
#define SUMMER_NET_ACCEPTOR

#include <boost/noncopyable.hpp>

#include <SummerNet/net/Socket.h>
#include <SummerNet/net/Channel.h>

#include <functional>
#include <atomic>

namespace summer
{
    namespace net
    {
        class EventLoop;
        class InetAddress;

        class Acceptor
            : public boost::noncopyable
        {
        public:
            typedef std::function<void(int sockFd, const InetAddress & addr)> NewConnectionCallback;
        private:
            void HandleRead();

            EventLoop * loop_;
            Socket acceptSocket_;
            Channel acceptChannel_;
            NewConnectionCallback newConnectionCallback_;
            std::atomic<bool> listenning_;
        public:
            Acceptor(EventLoop * loop, const InetAddress & listenAddr, bool reusePort);
            ~Acceptor();

            void SetNewConnectionCallback(const NewConnectionCallback & callback)
            {
                newConnectionCallback_ = callback;
            }

            void Listen();

            bool IsListenning() const
            {
                return listenning_ == true;
            }
        };
    }
}

#endif