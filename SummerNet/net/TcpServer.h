#ifndef SUMMER_NET_TCP_SERVER
#define SUMMER_NET_TCP_SERVER

#include <boost/noncopyable.hpp>

#include <SummerNet/net/Types.h>

#include <memory>
#include <string>
#include <map>
#include <atomic>

namespace summer
{
    namespace net
    {
        class EventLoop;
        class InetAddress;
        class Acceptor;
        class EventLoopThreadPool;

        class TcpServer
            : public boost::noncopyable
        {
        private:
            EventLoop * loop_;
            std::string name_;
            std::unique_ptr<Acceptor> acceptor_;
            std::unique_ptr<EventLoopThreadPool> threadPool_;
            int connectionId_;
            std::atomic<bool> start_;

            typedef std::map<std::string, TcpConnectionPtr> ConnectionMap;
            ConnectionMap connectionMap_;

            ConnectionCallback connectionCallback_;
            MessageCallback messageCallback_;
            WriteCompleteCallback writeCompleteCallback_;

            void NewConnection(int sockFd, const InetAddress & peerAddr);
            void RemoveConnection(const TcpConnectionPtr & connection);
            void RemoveConnectionInLoop(const TcpConnectionPtr & connection);
        public:
            TcpServer(EventLoop * loop, const InetAddress & listenAddr, const std::string & name);
            ~TcpServer();

            void SetConnectionCallback(const ConnectionCallback & callback)
            {
                connectionCallback_ = callback;
            }

            void SetMessageCallback(const MessageCallback & callback)
            {
                messageCallback_ = callback;
            }

            void WriteCompleteCallback(const WriteCompleteCallback & callback)
            {
                writeCompleteCallback_ = callback;
            }

            void SetThreadNum(int threadNum);
            void Start();
        };
    }
}

#endif