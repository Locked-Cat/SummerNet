#ifndef SUMMER_NET_CHANNEL
#define SUMMER_NET_CHANNEL

#include <boost/noncopyable.hpp>

#include <functional>
#include <memory>

namespace summer
{
    namespace net
    {
        class EventLoop;

        class Channel
            : boost::noncopyable
        {
        public:
            typedef std::function<void()> EventCallback;
        private:
            EventLoop * loop_;
            const int fd_;
            short events_;
            short revents_;
            int index_;
            bool eventHandling_;
            bool tied_;
            std::weak_ptr<void> tiedPtr_;

            EventCallback readCallback_;
            EventCallback writeCallback_;
            EventCallback errorCallback_;
            EventCallback closeCallback_;

            void Update();
            void HandleEventsWithGuard();

            static const short NoneEvent;
            static const short ReadEvent;
            static const short WriteEvent;
        public:
            Channel(EventLoop * loop, int fd);
            ~Channel();

            int GetIndex() const
            {
                return index_;
            }

            void SetIndex(int index)
            {
                index_ = index;
            }

            int GetFd() const
            {
                return fd_;
            }

            EventLoop * GetOwnerEventLoop() const
            {
                return loop_;
            }

            short GetEvents() const
            {
                return events_;
            }

            void SetRevents(short revents)
            {
                revents_ = revents;
            }

            void EnableReading()
            {
                events_ |= ReadEvent;
                Update();
            }

            void EnableWriting()
            {
                events_ |= WriteEvent;
                Update();
            }

            void DisableAll()
            {
                events_ = NoneEvent;
                Update();
            }

            void DisableReading()
            {
                events_ &= ~ReadEvent;
                Update();
            }

            void DisableWriting()
            {
                events_ &= ~WriteEvent;
                Update();
            }

            void SetReadCallback(const EventCallback & callback)
            {
                readCallback_ = callback;
            }

            void SetWriteCallback(const EventCallback & callback)
            {
                writeCallback_ = callback;
            }

            void SetCloseCallback(const EventCallback & callback)
            {
                closeCallback_ = callback;
            }

            void SetErrorCallback(const EventCallback & callback)
            {
                errorCallback_ = callback;
            }

            bool IsNoneEvent() const 
            {
                return events_ == NoneEvent;
            }

            bool IsWriting() const
            {
                return events_ & WriteEvent;
            }

            bool IsReading() const
            {
                return events_ & ReadEvent;
            }

            void HandleEvents();
            void Remove();
            void Tie(const std::shared_ptr<void> & tiedPtr)
            {
                tiedPtr_ = tiedPtr;
                tied_ = true;
            }
        };
    }
}

#endif