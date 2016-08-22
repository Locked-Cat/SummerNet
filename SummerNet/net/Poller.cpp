#include <SummerNet/net/Poller.h>
#include <SummerNet/net/poller/PollPoller.h>

#include <cstdlib>

using namespace summer::net;

Poller * Poller::GetPoller(EventLoop * ownerLoop)
{
    if(::getenv("SUMMER_NET_USE_POLL"))
    {
        return new PollPoller(ownerLoop);
    }

    return new PollPoller(ownerLoop);
}