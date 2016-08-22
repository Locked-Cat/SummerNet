#include <SummerNet/net/Channel.h>
#include <SummerNet/base/Logging.h>
#include <SummerNet/net/EventLoop.h>

#include <poll.h>
#include <cassert>

using namespace summer::net;

Channel::Channel(EventLoop * loop, int fd)
    : loop_(loop)
    , fd_(fd)
    , events_(0)
    , revents_(0)
    , index_(-1)
    , eventHandling_(false)
    , tied_(false)
{
}

Channel::~Channel()
{
    assert(eventHandling_ == false);
}

void Channel::Update()
{
    loop_->UpdateChannel(*this);
}

void Channel::Remove()
{
    assert(IsNoneEvent());
    loop_->RemoveChannel(*this);
}

void Channel::HandleEvents()
{
    if(tied_ == true)
    {
        auto guard = tiedPtr_.lock();
        if(guard)
        {
            HandleEventsWithGuard();
        }
    }
    else
    {
        HandleEventsWithGuard();
    }
}

void Channel::HandleEventsWithGuard()
{
    eventHandling_ = true;

    if(revents_ & POLLNVAL)
    {
        LOG_WARNING << "Channel::HandleEvent(): POLLNVAL";
    } 

    if(revents_ & (POLLERR | POLLNVAL))
    {
        if(errorCallback_)
        {
            errorCallback_();
        }
    }

    if((revents_ & POLLHUP) && !(revents_ & POLLIN))
    {
        LOG_WARNING << "Channel::HandleEvents(): POLLHUP";
        if(closeCallback_)
        {
            closeCallback_();
        }
    }

    if(revents_ & ReadEvent)
    {
        if(readCallback_)
        {
            readCallback_();
        }
    }

    if(revents_ & WriteEvent)
    {
        if(writeCallback_)
        {
            writeCallback_();
        }
    }

    eventHandling_ = false;
}

const short Channel::NoneEvent = 0;
const short Channel::ReadEvent = (POLLIN | POLLPRI);
const short Channel::WriteEvent = (POLLOUT | POLLWRBAND);