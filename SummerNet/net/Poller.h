#ifndef SUMMER_NET_POLLER
#define SUMMER_NET_POLLER

#include <boost/noncopyable.hpp>

#include <SummerNet/net/EventLoop.h>

#include <vector>
#include <chrono>
#include <map>
#include <memory>

namespace summer
{
    namespace net
    {
        class Channel;

        class Poller
        {
        public:
            typedef std::chrono::system_clock::time_point TimeStamp;
            typedef std::chrono::milliseconds TimeSpan;
            typedef std::vector<Channel *> ChannelList;

            explicit Poller(EventLoop * ownerLoop)
                : ownerLoop_(ownerLoop)
            {
            }

            virtual ~Poller()
            {
            }

            EventLoop * GetOwnerEventLoop() const
            {
                return ownerLoop_;
            }

            virtual TimeStamp Poll(TimeSpan & timeout, ChannelList & activeChannels) = 0;
            virtual void UpdateChannel(Channel & channel) = 0;
            virtual void RemoveChannel(Channel & channel) = 0;

            void AssertInLoopThread()
            {
                ownerLoop_->AssertInLoopThread();
            }

            static Poller * GetPoller(EventLoop * ownerLoop);
        protected:
            typedef std::map<int, Channel *> ChannelMap;
            ChannelMap channels_;
        private:
            EventLoop * ownerLoop_;
        };
    }
}

#endif