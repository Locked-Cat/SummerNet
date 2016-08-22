#ifndef SUMMER_NET_TIMER
#define SUMMER_NET_TIMER

#include <boost/noncopyable.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>

#include <SummerNet/net/Types.h>
#include <SummerNet/base/Logging.h>

#include <functional>
#include <chrono>
#include <thread>
#include <unistd.h>
#include <cstdint>
#include <atomic>

namespace summer
{
    namespace net
    {
        class Timer
            : public boost::noncopyable
        {
        private:
            const TimerCallback callback_;
            Timestamp expiration_;
            Timespan interval_;
            bool repeat_;
            uint64_t id_;
            bool cancelled;

            typedef boost::asio::basic_waitable_timer<std::chrono::system_clock> system_clock; 
            boost::asio::io_service io_;
            system_clock timer_;
            int pipeFd_;

            void HandleTimeout(const boost::system::error_code & error)
            {
                if(!error)
                {
                    auto n = ::write(pipeFd_, &id_, sizeof(id_));
                    if(n != sizeof(id_))
                    {
                        LOG_ERROR << "Timer::HandleTimeout writes " << n << " bytes instead of 8!"; 
                    }

                    if(IsRepeat())
                    {
                        timer_.expires_at(Clock::now() + interval_);
                        timer_.async_wait(boost::bind(&Timer::HandleTimeout, this, boost::asio::placeholders::error));
                    }
                }
                else
                {
                    LOG_INFO << "Timer cancelled.";
                }
            }
        public:
            Timer(const TimerCallback & callback, Timestamp when, Timespan interval, int pipeFd, uint64_t id)
                : callback_(callback)
                , expiration_(when)
                , interval_(interval)
                , repeat_(interval_ != Timespan::zero())
                , id_(id)
                , cancelled(false)
                , io_()
                , timer_(io_, when)
                , pipeFd_(pipeFd)
            {
                timer_.async_wait(boost::bind(&Timer::HandleTimeout, this, boost::asio::placeholders::error));
                std::thread t([this]()
                    {
                        io_.run();
                    });
                t.detach();
            } 

            Timestamp Expiration() const
            {
                return expiration_;
            }

            bool IsRepeat() const
            {
                return repeat_;
            }

            void Cancel()
            {
                timer_.cancel();
                cancelled = true;
            }

            bool IsCancelled() const
            {
                return cancelled;
            }

            void Run() const
            {
                callback_();
            }
        };
    }
}

#endif