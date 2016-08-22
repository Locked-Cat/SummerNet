#include <SummerNet/net/Acceptor.h>
#include <SummerNet/net/EventLoop.h>
#include <SummerNet/net/InetAddress.h>
#include <SummerNet/net/SocketOps.h>

using namespace summer;
using namespace summer::net;

Acceptor::Acceptor(EventLoop * loop, const InetAddress & listenAddr, bool reusePort)
    : loop_(loop)
    , acceptSocket_(sockets::CreateNonblockingSocket())
    , acceptChannel_(loop_, acceptSocket_.GetFd())
    , listenning_(false)
{
    acceptSocket_.SetReuseAddr(true);
    acceptSocket_.SetReusePort(reusePort);
    acceptSocket_.Bind(listenAddr);
    acceptChannel_.SetReadCallback(std::bind(&Acceptor::HandleRead, this));
}

Acceptor::~Acceptor()
{
    acceptChannel_.DisableAll();
    acceptChannel_.Remove();
}

void Acceptor::HandleRead()
{
    loop_->AssertInLoopThread();
    InetAddress peerAddr;
    auto connFd = acceptSocket_.Accept(peerAddr);
    if(connFd >= 0)
    {
        if(newConnectionCallback_)
        {
            newConnectionCallback_(connFd, peerAddr);
        }
        else
        {
            sockets::Close(connFd);
        }
    }
    else
    {
        LOG_ERROR << "Acceptor::HandleRead()";
    }
}

void Acceptor::Listen()
{
    loop_->AssertInLoopThread();
    listenning_ = true;
    acceptSocket_.Listen();
    acceptChannel_.EnableReading();
}