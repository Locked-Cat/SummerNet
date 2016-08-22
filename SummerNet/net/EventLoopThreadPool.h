#ifndef SUMMER_NET_EVENT_LOOP_THREAD_POOL
#define SUMMER_NET_EVENT_LOOP_THREAD_POOL

#include <boost/noncopyable.hpp>
#include <vector>

namespace summer
{
    namespace net
    {
        class EventLoop;
        class EventLoopThread;

        class EventLoopThreadPool
            : public boost::noncopyable
        {
        private:
            EventLoop * baseLoop_;
            std::vector<EventLoopThread *> threads_;
            std::vector<EventLoop *> loops_;
            bool start_;
            int threadNum_;
            int next_;
        public:
            EventLoopThreadPool(EventLoop * baseLoop);
            ~EventLoopThreadPool();

            int GetThreadNum() const
            {
                return threadNum_;
            }

            void SetThreadNum(int num)
            {
                threadNum_ = num;
            } 

            void Start();
            
            bool Started() const
            {
                return start_;
            }

            EventLoop * GetNextLoop();
        };
    }
}

#endif