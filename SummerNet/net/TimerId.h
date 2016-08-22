#ifndef SUMMER_NET_TIMER_ID
#define SUMMER_NET_TIMER_ID

#include <atomic>
#include <cstdint>

namespace summer
{
    namespace net
    {
        class TimerId
        {
            friend class TimerQueue;
        private:
            static std::atomic<uint64_t> sequence_;
            static uint64_t IncrementSequenceAndGet();
            
            uint64_t id_;
            uint64_t GetId() const
            {
                return id_;
            }
        public:
            TimerId();
        };
    }
}

#endif