#ifndef SUMMER_NET_COUNTDOWNLATCH
#define SUMMER_NET_COUNTDOWNLATCH

#include <boost/noncopyable.hpp>

#include <SummerNet/base/Mutex.h>
#include <SummerNet/base/Condition.h>

namespace summer
{
    class CountDownLatch
    {
    private:
        mutable Mutex mutex_;
        Condition condition_;
        int count_;
    public:
        explicit CountDownLatch(int count);
        
        void Wait();
        
        void CountDown();
        
        int GetCount() const;
    };
}

#endif