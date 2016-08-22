#include <SummerNet/net/TimerQueue.h>
#include <SummerNet/net/TimerId.h>
#include <SummerNet/net/Timer.h>
#include <SummerNet/base/Logging.h>
#include <SummerNet/net/Channel.h>
#include <SummerNet/net/EventLoop.h>

#include <unistd.h>
#include <functional>
#include <cassert>

using namespace summer::net;

TimerQueue::TimerQueue(EventLoop * loop)
    : loop_(loop)
    , io_()
    , timerList_()
{
    if(::pipe(pipeFds_) != 0)
        LOG_ERROR << "TimerQueue::TimerQueue(): pipe created failed!";
    
    timeChannel_ = std::make_unique<Channel>(loop_, pipeFds_[0]);
    timeChannel_->SetReadCallback(std::bind(&TimerQueue::HandleRead, this));
    timeChannel_->EnableReading();
}

TimerQueue::~TimerQueue()
{
    timeChannel_->DisableAll();
    timeChannel_->Remove();
    ::close(pipeFds_[0]);
    ::close(pipeFds_[1]);
}

void TimerQueue::HandleRead()
{
    loop_->AssertInLoopThread();
    uint64_t id;
    auto n = ::read(pipeFds_[0], &id, sizeof(id));
    if(n != sizeof(id))
    {
        LOG_ERROR << "TimerQueue::HandleRead: pipe reads " << n << " bytes instead of 8!";
    }
    
    auto timer = timerList_.find(id);
    assert(timer != timerList_.end());

    if(!timer->second->IsCancelled())
    {
        timer->second->Run();
    }

    if(!timer->second->IsRepeat())
    {
        timerList_.erase(timer);
    }
}

void TimerQueue::AddTimerInLoop(const TimerCallback & callback, Timestamp when, Timespan interval, uint64_t id)
{
    loop_->AssertInLoopThread();
    auto timer = std::make_unique<Timer>(callback, when, interval, pipeFds_[1], id);
    timerList_[id] = std::move(timer);
}

TimerId TimerQueue::AddTimer(const TimerCallback & callback, Timestamp when, Timespan interval)
{
    TimerId id;
    loop_->RunInLoop(std::bind(&TimerQueue::AddTimerInLoop, this, callback, when, interval, id.GetId()));
    return id;
}

void TimerQueue::CancelInLoop(TimerId id)
{
    loop_->AssertInLoopThread();

    auto timer = timerList_.find(id.GetId());
    assert(timer != timerList_.end());
    timer->second->Cancel();
}

void TimerQueue::Cancel(TimerId id)
{
    loop_->RunInLoop(std::bind(&TimerQueue::CancelInLoop, this, id));
}
