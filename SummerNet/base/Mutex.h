#ifndef SUMMER_NET_MUTEX
#define SUMMER_NET_MUTEX

#include <boost/noncopyable.hpp>

#include <SummerNet/base/Logging.h>

#include <thread>
#include <pthread.h>

namespace summer
{
    class Mutex
    {
        friend class Condition;
    private:
        pthread_mutex_t mutex_;
        std::thread::id holder_;
        
        void UnassignHolder()
        {
            holder_ = std::thread::id();
        }
        
        void AssignHolder()
        {
            holder_ = std::this_thread::get_id();
        }
        
        class UnassignGuard
        {
        private:
            Mutex & owner_;
        public:
            UnassignGuard(Mutex & owner)
                : owner_(owner)
            {
                owner_.UnassignHolder();
            }
            
            ~UnassignGuard()
            {
                owner_.AssignHolder();
            }
        };
        
        pthread_mutex_t * GetOriginalMutexPtr()
        {
            return &mutex_;
        }
    public:
        Mutex()
        {
            if(pthread_mutex_init(&mutex_, nullptr) != 0)
            {
                LOG_FATAL << "Mutex::Mutex()";
            }
        }
        
        ~Mutex()
        {
            if(pthread_mutex_destroy(&mutex_) != 0)
            {
                LOG_FATAL << "Mutex::~Mutex()";
            }
        }
        
        void Lock()
        {
            if(pthread_mutex_lock(&mutex_) != 0)
            {
                LOG_FATAL << "Mutex::Lock()";
            }
            AssignHolder();
        }
        
        void Unlock()
        {
            UnassignHolder();
            if(pthread_mutex_unlock(&mutex_) != 0)
            {
                LOG_FATAL << "Mutex::Unlock()";
            }
        }
        
        bool IsLockedByThisThread() const
        {
            return std::this_thread::get_id() == holder_;
        }
    };
    
    class MutexGuard
    {
    private:
        Mutex & mutex_;
    public:
        explicit MutexGuard(Mutex & mutex)
            : mutex_(mutex)
        {
            mutex_.Lock();
        }
        
        ~MutexGuard()
        {
            mutex_.Unlock();
        }
    };
    
    #define MutexGuard(x) static_assert(false, "Incorrect usage of MutexGuard class!")
}

#endif