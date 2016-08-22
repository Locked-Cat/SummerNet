#include <SummerNet/base/CountDownLatch.h>

using namespace summer;

CountDownLatch::CountDownLatch(int count)
    : mutex_()
    , condition_(mutex_)
    , count_(count)
{
}

void CountDownLatch::Wait()
{
    MutexGuard guard(mutex_);
    while(count_ >0)
    {
        condition_.Wait();
    }
}

void CountDownLatch::CountDown()
{
    MutexGuard guard(mutex_);
    --count_;
    if(count_ == 0)
    {
        condition_.NotifyAll();
    }
}

int CountDownLatch::GetCount() const
{
    MutexGuard guard(mutex_);
    return count_;
}