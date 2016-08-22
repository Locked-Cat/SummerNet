#ifndef SUMMER_NET_EVENT_LOOP_THREAD
#define SUMMER_NET_EVENT_LOOP_THREAD

#include <boost/noncopyable.hpp>

#include <SummerNet/base/Mutex.h>
#include <SummerNet/base/Condition.h>

#include <memory>
#include <thread>

namespace summer
{
    namespace net
    {
        class EventLoop;
        
        class EventLoopThread
            : public boost::noncopyable
        {
        private:
            void ThreadFunc();

            EventLoop * loop_;
            Mutex mutex_;
            Condition cond_;
            std::unique_ptr<std::thread> thread_;
        public:
            EventLoopThread();
            ~EventLoopThread();

            EventLoop * Start();
        };
    }
}

#endif