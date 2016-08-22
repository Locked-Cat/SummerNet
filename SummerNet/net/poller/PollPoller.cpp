#include <SummerNet/net/poller/PollPoller.h>
#include <SummerNet/net/Channel.h>
#include <SummerNet/net/Types.h>

#include <poll.h>
#include <cassert>
#include <algorithm>

using namespace summer::net;

PollPoller::PollPoller(EventLoop * ownerLoop)
    : Poller(ownerLoop)
{
}

PollPoller::TimeStamp PollPoller::Poll(TimeSpan & timeout, ChannelList & activeChannels)
{
    auto timeoutMs = timeout.count();
    auto numEvents = ::poll(pollFds_.data(), static_cast<nfds_t>(pollFds_.size()), static_cast<int>(timeoutMs));
    auto now = Clock::now();
    
    if(numEvents > 0)
    {
        LOG_INFO << numEvents << " events happended.";
        FillActiveChannels(numEvents, activeChannels);
    }
    else
    {
        if(numEvents == 0)
        {
            LOG_INFO << "Nothing happended.";
        }
        else
        {
            LOG_ERROR << "Poller::Poll()";
        }
    }

    return now;
}

void PollPoller::FillActiveChannels(int numEvents, ChannelList & activeChannels)
{
    for(auto pfd = pollFds_.begin(); pfd != pollFds_.end() && numEvents > 0; ++pfd)
    {
        if(pfd->revents > 0)
        {
            auto channel = channels_.find(pfd->fd);
            assert(channel != channels_.end());

            channel->second->SetRevents(pfd->revents);
            activeChannels.push_back(channel->second);
            --numEvents;
        }
    }
}

void PollPoller::UpdateChannel(Channel & channel)
{
    AssertInLoopThread();
    LOG_INFO << "fd = " << channel.GetFd() << " events " << channel.GetEvents();
    if(channel.GetIndex() < 0)
    {
        assert(channels_.find(channel.GetFd()) == channels_.end());
        
        channel.SetIndex(static_cast<int>(pollFds_.size()));

        struct pollfd pfd;
        pfd.fd = channel.GetFd();
        pfd.events = channel.GetEvents();
        pfd.revents = 0;

        pollFds_.push_back(pfd);
        channels_[pfd.fd] = &channel;
    }
    else
    {
        assert(channels_.find(channel.GetFd()) != channels_.end());
        assert(channels_[channel.GetFd()] == &channel);

        auto index = channel.GetIndex();
        assert(index >= 0 && static_cast<size_t>(index) < pollFds_.size());

        auto & pfd = pollFds_[static_cast<PollFdList::size_type>(index)];
        assert(pfd.fd == channel.GetFd() || pfd.fd == -channel.GetFd() - 1);
        pfd.events = channel.GetEvents();
        pfd.revents = 0;

        if(channel.IsNoneEvent())
        {
            pfd.fd = -channel.GetFd() - 1;
        }
    }
}

void PollPoller::RemoveChannel(Channel & channel)
{
    AssertInLoopThread();

    assert(channels_.find(channel.GetFd()) != channels_.end());
    assert(channels_[channel.GetFd()] == &channel);
    assert(channel.IsNoneEvent());

    auto index = channel.GetIndex();
    assert(index >= 0 && static_cast<size_t>(index) < pollFds_.size());
    auto & pfd = pollFds_[static_cast<size_t>(index)];
    assert(pfd.fd == -channel.GetFd() - 1 && pfd.events == channel.GetEvents());
    (void)pfd;
    auto n = channels_.erase(channel.GetFd());
    assert(n == 1);
    (void)n;
    
    if(static_cast<size_t>(index) == pollFds_.size() - 1)
    {
        pollFds_.pop_back();
    }
    else
    {
        auto fd = pollFds_.back().fd;
        if(fd < 0)
        {
            fd = -fd - 1;
        }
        channels_[fd]->SetIndex(index);
        std::iter_swap(pollFds_.begin() + index, pollFds_.end() - 1);
        pollFds_.pop_back();
    }
}