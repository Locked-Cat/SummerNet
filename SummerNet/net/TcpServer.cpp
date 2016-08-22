#include <SummerNet/net/TcpServer.h>
#include <SummerNet/net/EventLoop.h>
#include <SummerNet/net/Acceptor.h>
#include <SummerNet/base/Logging.h>
#include <SummerNet/net/SocketOps.h>
#include <SummerNet/net/InetAddress.h>
#include <SummerNet/net/TcpConnection.h>
#include <SummerNet/net/EventLoopThreadPool.h>

#include <cstdio>
#include <functional>

using namespace summer;
using namespace summer::net;

TcpServer::TcpServer(EventLoop * loop, const InetAddress & listenAddr, const std::string & name)
    : loop_(loop)
    , name_(name)
    , acceptor_(new Acceptor(loop, listenAddr, true))
    , threadPool_(new EventLoopThreadPool(loop))
    , connectionId_(0)
    , start_(false)
{
    acceptor_->SetNewConnectionCallback(std::bind(&TcpServer::NewConnection, this, std::placeholders::_1, std::placeholders::_2));
}

TcpServer::~TcpServer()
{
    loop_->AssertInLoopThread();
    LOG_INFO << "TcpServer [" << name_ << "] destructing";

    for(auto it = connectionMap_.begin(); it != connectionMap_.end(); ++it)
    {
        auto connection = it->second;
        (it->second).reset();
        loop_->RunInLoop(std::bind(&TcpConnection::ConnectionDestroyed, connection));
        connection.reset();
    }
}

void TcpServer::SetThreadNum(int threadNum)
{
    assert(0 <= threadNum);
    threadPool_->SetThreadNum(threadNum);
}

void TcpServer::Start()
{
    assert(false == start_);
    threadPool_->Start();
    assert(!acceptor_->IsListenning());
    loop_->RunInLoop(std::bind(&Acceptor::Listen, acceptor_.get()));
}

void TcpServer::NewConnection(int sockFd, const InetAddress & peerAddr)
{
    loop_->AssertInLoopThread();
    auto ioLoop = threadPool_->GetNextLoop();
    char buf[64];
    ::snprintf(buf, sizeof(buf), "-%d", connectionId_++);
    std::string connectionName = name_ + buf;

    LOG_INFO << "TcpServer::NewConnection [" << name_ << "] - new connection [" << connectionName << "] from " << peerAddr.ToIpPort();
    auto localAddr = InetAddress(sockets::GetLocalAddr(sockFd));
    auto connection = std::make_shared<TcpConnection>(ioLoop, sockFd, connectionName, localAddr, peerAddr);
    connectionMap_[connectionName] = connection;
    connection->SetConnectionCallback(connectionCallback_);
    connection->SetMessageCallback(messageCallback_);
    connection->SetCloseCallback(std::bind(&TcpServer::RemoveConnection, this, std::placeholders::_1));
    ioLoop->RunInLoop(std::bind(&TcpConnection::ConnectionEstablished, connection));
}

void TcpServer::RemoveConnection(const TcpConnectionPtr & connection)
{
    loop_->RunInLoop(std::bind(&TcpServer::RemoveConnectionInLoop, this, connection));
}

void TcpServer::RemoveConnectionInLoop(const TcpConnectionPtr & connection)
{
    loop_->AssertInLoopThread();
    LOG_INFO << "TcpServer::RemoveConnection [" << name_ << "] - connection " << connection->GetName();
    auto n = connectionMap_.erase(connection->GetName());
    assert(n == 1); (void)n;
    auto ioLoop = connection->GetOwnerEventLoop();
    ioLoop->RunInLoop(std::bind(&TcpConnection::ConnectionDestroyed, connection)); 
}