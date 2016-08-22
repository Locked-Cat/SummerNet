#include <SummerNet/net/EventLoopThreadPool.h>
#include <SummerNet/net/EventLoopThread.h>
#include <SummerNet/net/EventLoop.h>

#include <cassert>

using namespace summer;
using namespace summer::net;

EventLoopThreadPool::EventLoopThreadPool(EventLoop * baseLoop)
    : baseLoop_(baseLoop)
    , start_(false)
    , threadNum_(0)
    , next_(0)
{
}

EventLoopThreadPool::~EventLoopThreadPool()
{
    if(!loops_.empty())
    {
        for(size_t i = 0; i < loops_.size(); ++i)
        {
            loops_[i]->Quit();
        }
    }
}

void EventLoopThreadPool::Start()
{
    baseLoop_->AssertInLoopThread();
    assert(!start_);

    start_ = true;
    for(auto i = 0; i < threadNum_; ++i)
    {
        auto t = new EventLoopThread();
        threads_.push_back(t);
        loops_.push_back(t->Start());
    }
}

EventLoop * EventLoopThreadPool::GetNextLoop()
{
    baseLoop_->AssertInLoopThread();
    assert(start_);
    auto loop = baseLoop_;

    if(!loops_.empty())
    {
        loop = loops_[static_cast<size_t>(next_++)];
        if(static_cast<size_t>(next_) >= loops_.size())
        {
            next_ = 0;
        }
    }
    return loop;
}