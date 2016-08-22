#include <SummerNet/net/EventLoop.h>
#include <SummerNet/net/Channel.h>
#include <SummerNet/net/Poller.h>
#include <SummerNet/net/TimerQueue.h>
#include <SummerNet/base/Logging.h>

#include <cassert>
#include <cstdint>
#include <unistd.h>
#include <signal.h>

using namespace summer::net;

namespace
{
    __thread EventLoop * tLoopInThisThread = nullptr;

    class IgnoreSigPipe
    {
    public:
        IgnoreSigPipe()
        {
            ::signal(SIGPIPE, SIG_IGN);
            //LOG_INFO << "Ignore SIGPIPE";
        }
    };
}

EventLoop::EventLoop()
    : looping_(false)
    , quit_(false)
    , id_(std::this_thread::get_id())
    , handlingPendingFunctors_(false)
    , poller_(Poller::GetPoller(this))
    , timerQueue_(new TimerQueue(this))
{
    if(tLoopInThisThread)
    {
        LOG_FATAL << "Another EventLoop exist in current thread!";
    }
    else
    {
        tLoopInThisThread = this;
    }

    if(::pipe(wakeupFds_) != 0)
    {
        LOG_FATAL << "Can't create wakeup pipe!";
    }
    else
    {
        wakeupChannel_.reset(new Channel(this, wakeupFds_[0]));
        wakeupChannel_->SetReadCallback(std::bind(&EventLoop::HandleRead, this));
        wakeupChannel_->EnableReading();
    }
}

EventLoop::~EventLoop()
{
    assert(looping_);

    wakeupChannel_->DisableAll();
    wakeupChannel_->Remove();
    
    ::close(wakeupFds_[0]);
    ::close(wakeupFds_[1]);

    tLoopInThisThread = nullptr;
}

void EventLoop::Wakeup()
{
    uint64_t one = 1;
    auto n = ::write(wakeupFds_[1], &one, sizeof(one));
    if(n != sizeof(one))
    {
        LOG_ERROR << "EventLoop::Wakeup() writes " << n << " bytes instead of 8";
    }
}

void EventLoop::HandleRead()
{
    uint64_t one = 1;
    auto n = ::read(wakeupFds_[0], &one, sizeof(one));
    if(n != sizeof(one))
    {
        LOG_ERROR << "EventLoop::HandleRead() reads " << n << " bytes instead of 8";
    }
}

void EventLoop::HandlePendingFunctors()
{
    std::vector<Functor> functors;
    handlingPendingFunctors_ = true;

    {
        MutexGuard guard(mutex_);
        functors.swap(pendingFunctors_);
    }

    for(auto & functor: functors)
    {
        functor();
    }

    handlingPendingFunctors_ = false;
}

void EventLoop::Loop()
{
    assert(!looping_);
    AssertInLoopThread();

    quit_ = false;
    looping_ = true;
    while(!quit_)
    {
        activeChannels_.clear();
        poller_->Poll(PollTimeout, activeChannels_);

        for(auto & channel: activeChannels_)
        {
            channel->HandleEvents();
        }

        HandlePendingFunctors();
    }

    LOG_INFO << "EventLoop stop looping.";
    looping_ = false;
}

void EventLoop::RunInLoop(const Functor & functor)
{
    if(IsInLoopThread())
    {
        functor();
    }
    else
    {
        QueueInLoop(functor);
    }
}

void EventLoop::QueueInLoop(const Functor & functor)
{
    {
        MutexGuard guard(mutex_);
        pendingFunctors_.push_back(functor);
    }

    if(!IsInLoopThread() || handlingPendingFunctors_)
    {
        Wakeup();
    }
}

void EventLoop::UpdateChannel(Channel & channel)
{
    assert(channel.GetOwnerEventLoop() == this);
    AssertInLoopThread();
    poller_->UpdateChannel(channel);
}

void EventLoop::RemoveChannel(Channel & channel)
{
    assert(channel.GetOwnerEventLoop() == this);
    AssertInLoopThread();
    poller_->RemoveChannel(channel);
}


TimerId EventLoop::RunAt(const Timestamp & time, const TimerCallback & callback)
{
    return timerQueue_->AddTimer(callback, time, Timespan::zero());
}

TimerId EventLoop::RunAfter(const Timespan & delay, const TimerCallback & callback)
{
    return timerQueue_->AddTimer(callback, Clock::now() + delay, Timespan::zero());
}

TimerId EventLoop::RunEvery(const Timespan & interval, const TimerCallback & callback)
{
    return timerQueue_->AddTimer(callback, Clock::now() + interval, interval);
}

void EventLoop::Cancel(TimerId id)
{
    timerQueue_->Cancel(id);
}

std::chrono::milliseconds EventLoop::PollTimeout(10000);
