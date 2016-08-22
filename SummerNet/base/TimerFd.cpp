#include <SummerNet/base/TimerFd.h>
#include <SummerNet/base/Logging.h>

#include <thread>
#include <unistd.h>
#include <poll.h>

using namespace summer;

TimerFd::TimerFd()
    : isTiming_(false)
{
    if(::pipe(pipeFd_) == -1)
    {
        LOG_FATAL << "TimerFd not available.";
    }
}

TimerFd::~TimerFd()
{
    ::close(pipeFd_[1]);
    ::close(pipeFd_[0]);
}

int TimerFd::GetFd() const
{
    return pipeFd_[0];
}

void TimerFd::SetTime(const std::chrono::milliseconds & timeout)
{
    if(isTiming_)
    {
        LOG_ERROR << "Timer is running.";
        return;
    }
    
    auto timeoutMs = static_cast<int>(timeout.count());
    isTiming_ = true;
    std::thread timer([this, timeoutMs]()
        {
            ::poll(nullptr, 0, timeoutMs);
            ::write(pipeFd_[1], " ", 1);
            isTiming_ = false;
        });
    timer.detach();
}

