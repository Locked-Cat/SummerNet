#ifndef SUMMER_NET_POLL_POLLER
#define SUMMER_NET_POLL_POLLER

#include <SummerNet/net/EventLoop.h>
#include <SummerNet/net/Poller.h>

#include <vector>

struct pollfd;

namespace summer
{
    namespace net
    {   
        class PollPoller
            : public Poller
        {
        public:
            PollPoller(EventLoop * loop);

            virtual TimeStamp Poll(TimeSpan & timeout, ChannelList & activeChannels) override;
            virtual void UpdateChannel(Channel & channel) override;
            virtual void RemoveChannel(Channel & channel) override;
        private:
            typedef std::vector<struct pollfd> PollFdList;
            void FillActiveChannels(int numEvents, ChannelList & activeChannels);
            
            PollFdList pollFds_;
        };
    }
}
#endif