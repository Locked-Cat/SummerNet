#ifndef SUMMER_NET_CONDITION
#define SUMMER_NET_CONDITION

#include <boost/noncopyable.hpp>

#include <SummerNet/base/Mutex.h>
#include <SummerNet/base/Logging.h>

#include <pthread.h>
#include <mutex>
#include <chrono>

namespace summer
{
    class Condition
        : boost::noncopyable
    {
    private:
        Mutex & mutex_;
        pthread_cond_t cond_;
    public:
        explicit Condition(Mutex & mutex)
            : mutex_(mutex)
        {
            if(pthread_cond_init(&cond_, nullptr) != 0)
            {
                LOG_FATAL << "Condition::Condition()";
            }
        }
        
        ~Condition()
        {
            if(pthread_cond_destroy(&cond_) != 0)
            {
                LOG_FATAL << "Condition::~Condition()";
            }
        }
        
        void Wait()
        {
            Mutex::UnassignGuard guard(mutex_);
            if(pthread_cond_wait(&cond_, mutex_.GetOriginalMutexPtr()) != 0)
            {
                LOG_FATAL << "Condtion::Wait()";
            }
        }
        
        void NotifyOne()
        {
            if(pthread_cond_signal(&cond_) != 0)
            {
                LOG_FATAL << "Condtion::NotifyOne()";
            }
        }
        
        void NotifyAll()
        {
            if(pthread_cond_broadcast(&cond_) != 0)
            {
                LOG_FATAL << "Condtion::NotifyAll()";
            }
        }
    };
}

#endif