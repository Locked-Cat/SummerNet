#ifndef SUMMER_NET_EVENTLOOP
#define SUMMER_NET_EVENTLOOP

#include <boost/noncopyable.hpp>

#include <SummerNet/base/Logging.h>
#include <SummerNet/base/Mutex.h>
#include <SummerNet/net/Types.h>
#include <SummerNet/net/TimerId.h>

#include <thread>
#include <atomic>
#include <memory>
#include <vector>
#include <chrono>
#include <functional>

namespace summer
{
    namespace net
    {
        class Channel;
        class Poller;
        class TimerQueue;
        
        class EventLoop
            : public boost::noncopyable
        {
        public:
            typedef std::function<void(void)> Functor;
        private:
            std::atomic<bool> looping_;
            std::atomic<bool> quit_;
            const std::thread::id id_;
            std::atomic<bool> handlingPendingFunctors_;
            std::unique_ptr<Poller> poller_;
            std::unique_ptr<TimerQueue> timerQueue_;

            int wakeupFds_[2];
            std::unique_ptr<Channel> wakeupChannel_;

            Mutex mutex_;
            std::vector<Functor> pendingFunctors_;

            typedef std::vector<Channel *> ChannelList;
            ChannelList activeChannels_;

            void HandleRead();
            void Wakeup();
            void HandlePendingFunctors();
            void QueueInLoop(const Functor & functor);
        public:
            EventLoop();
            ~EventLoop();
            
            void Loop();
            void UpdateChannel(Channel & channel);
            void RemoveChannel(Channel & channel);
            void RunInLoop(const Functor & functor);

            void AssertInLoopThread()
            {
                if(!IsInLoopThread())
                {
                    LOG_FATAL << "Not in loop thread!";
                }
            }

            bool IsInLoopThread()
            {
                return std::this_thread::get_id() == id_;
            }

            void Quit()
            {
                quit_ = true;
            }

            TimerId RunAt(const Timestamp & time, const TimerCallback & callback);
            TimerId RunAfter(const Timespan & delay, const TimerCallback & callback);
            TimerId RunEvery(const Timespan & interval, const TimerCallback & callback);
            void Cancel(TimerId);

            static std::chrono::milliseconds PollTimeout;
        };
    }
}

#endif