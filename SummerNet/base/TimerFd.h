#ifndef SUMMER_NET_TIMERFD
#define SUMMER_NET_TIMERFD

#include <boost/noncopyable.hpp>

#include <chrono>
#include <thread>
#include <atomic>

namespace summer
{
    class TimerFd
        : public boost::noncopyable
    {
    private:
        int pipeFd_[2];
        std::atomic<bool> isTiming_;
    public:
        TimerFd();
        ~TimerFd();

        int GetFd() const;
        void SetTime(const std::chrono::milliseconds & timeout);
    };
}

#endif