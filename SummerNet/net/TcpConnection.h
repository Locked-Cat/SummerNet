#ifndef SUMMER_NET_TCP_CONNECTION
#define SUMMER_NET_TCP_CONNECTION

#include <boost/noncopyable.hpp>

#include <SummerNet/net/InetAddress.h>
#include <SummerNet/net/Types.h>
#include <SummerNet/net/Buffer.h>

#include <memory>
#include <string>
#include <atomic>

namespace summer
{
    namespace net
    {
        class Socket;
        class Channel;
        class EventLoop;

        class TcpConnection
            : public boost::noncopyable
            , public std::enable_shared_from_this<TcpConnection>
        {
        private:
            enum State
            {
                DISCONNECTING,
                DISCONNECTED,
                CONNECTING,
                CONNECTED
            };

            EventLoop * loop_;
            std::atomic<State> state_;

            std::unique_ptr<Socket> socket_;
            std::unique_ptr<Channel> channel_;
            std::string name_;
            const InetAddress localAddr_;
            const InetAddress peerAddr_;

            ConnectionCallback connectionCallback_;
            MessageCallback messageCallback_;
            CloseCallback closeCallback_;
            WriteCompleteCallback writeCompleteCallback_;

            Buffer inputBuffer_;
            Buffer outputBuffer_;
            
            void HandleRead();
            void HandleWrite();
            void HandleClose();
            void HandleError();
            void ShutdownInLoop();
            void SendInLoop(const void * data, size_t len);
        public:
            TcpConnection(EventLoop * loop, int sockFd, const std::string & name, const InetAddress & localAddr, const InetAddress & peerAddr);
            void ConnectionEstablished();
            void ConnectionDestroyed();

            void SetConnectionCallback(const ConnectionCallback & callback)
            {
                connectionCallback_ = callback;
            }

            void SetMessageCallback(const  MessageCallback & callback)
            {
                messageCallback_ = callback;
            }

            void SetCloseCallback(const CloseCallback & callback)
            {
                closeCallback_ = callback;
            }

            void SetWriteCompleteCallback(const WriteCompleteCallback & callback)
            {
                writeCompleteCallback_ = callback;
            }

            std::string GetName() const
            {
                return name_;
            }

            EventLoop * GetLoop() const
            {
                return loop_;
            }

            bool IsConnected() const
            {
                return state_ == CONNECTED;
            }

            bool IsDisconnected() const
            {
                return state_ == DISCONNECTED;
            }

            InetAddress GetPeerAddress() const
            {
                return peerAddr_;
            }

            EventLoop * GetOwnerEventLoop() const
            {
                return loop_;
            }

            void SetTcpNoDelay(bool on);
            void SetKeepAlive(bool on);

            void Shutdown();
            void Send(const std::string & message);
            void Send(const void * data, size_t len);
        };
    }
}

#endif