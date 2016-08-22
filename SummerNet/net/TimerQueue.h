#ifndef SUMMER_NET_TIMER_QUEUE
#define SUMMER_NET_TIMER_QUEUE

#include <boost/noncopyable.hpp>
#include <boost/asio.hpp>

#include <SummerNet/net/Types.h>

#include <utility>
#include <memory>
#include <map>
#include <cstdint>

namespace summer
{
    namespace net
    {
        class EventLoop;
        class Timer;
        class TimerId;
        class Channel;

        class TimerQueue
            : public boost::noncopyable
        {
        private:
            typedef std::map<uint64_t, std::unique_ptr<Timer>> TimerList;

            EventLoop * loop_;
            boost::asio::io_service io_;
            TimerList timerList_;
            int pipeFds_[2];
            std::unique_ptr<Channel> timeChannel_;

            void HandleRead();
            void AddTimerInLoop(const TimerCallback & callback, Timestamp when, Timespan interval, uint64_t id);
            void CancelInLoop(TimerId id);
        public:
            TimerQueue(EventLoop * loop);
            ~TimerQueue();

            TimerId AddTimer(const TimerCallback & callback, Timestamp when, Timespan interval);
            void Cancel(TimerId timerId);
        };
    }
}

#endif