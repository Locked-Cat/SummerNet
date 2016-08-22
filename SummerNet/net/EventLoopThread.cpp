#include <SummerNet/net/EventLoopThread.h>
#include <SummerNet/net/EventLoop.h>

#include <cassert>
#include <thread>
#include <functional>

using namespace summer;
using namespace summer::net;

EventLoopThread::EventLoopThread()
    : loop_(nullptr)
    , mutex_()
    , cond_(mutex_)
{
}

EventLoopThread::~EventLoopThread()
{
    loop_->Quit();
    if(thread_->joinable())
    {
        thread_->join();
    }
}

EventLoop * EventLoopThread::Start()
{
    assert(thread_ == nullptr);   
    thread_.reset(new std::thread(std::bind(&EventLoopThread::ThreadFunc, this)));

    {
        MutexGuard guard(mutex_);
        while(loop_ == nullptr)
        {
            cond_.Wait();
        }
    }

    return loop_;
}

void EventLoopThread::ThreadFunc()
{
    EventLoop loop;

    {
        MutexGuard guard(mutex_);
        loop_ = &loop;
        cond_.NotifyAll();
    }

    loop.Loop();
    loop_ = nullptr;
}