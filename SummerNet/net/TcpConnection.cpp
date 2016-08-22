#include <SummerNet/net/TcpConnection.h>
#include <SummerNet/net/Channel.h>
#include <SummerNet/net/Socket.h>
#include <SummerNet/net/EventLoop.h>
#include <SummerNet/net/SocketOps.h>
#include <SummerNet/base/Logging.h>

#include <errno.h>
#include <functional>
#include <cassert>

using namespace summer;
using namespace summer::net;

TcpConnection::TcpConnection(EventLoop * loop, int sockFd, const std::string & name, const InetAddress & localAddr, const InetAddress & peerAddr)
    : loop_(loop)
    , state_(CONNECTING)
    , socket_(new Socket(sockFd))
    , channel_(new Channel(loop, sockFd))
    , name_(name)
    , localAddr_(localAddr)
    , peerAddr_(peerAddr)
{
    channel_->SetReadCallback(std::bind(&TcpConnection::HandleRead, this));
    channel_->SetWriteCallback(std::bind(&TcpConnection::HandleWrite, this));
    channel_->SetCloseCallback(std::bind(&TcpConnection::HandleClose, this));
    channel_->SetErrorCallback(std::bind(&TcpConnection::HandleError, this));
}

void TcpConnection::ConnectionEstablished()
{
    loop_->AssertInLoopThread();
    assert(state_ == CONNECTING);
    state_ = CONNECTED;
    channel_->Tie(shared_from_this());
    channel_->EnableReading();

    connectionCallback_(shared_from_this());
}

void TcpConnection::ConnectionDestroyed()
{
    loop_->AssertInLoopThread();
    if(state_ == CONNECTED)
    {
        state_ = DISCONNECTED;
        channel_->DisableAll();
        connectionCallback_(shared_from_this());
    }
    channel_->Remove();
}

void TcpConnection::Send(const std::string & message)
{
    loop_->RunInLoop(std::bind(&TcpConnection::SendInLoop, shared_from_this(), message.data(), message.size()));
}

void TcpConnection::Send(const void * data, size_t len)
{
    loop_->RunInLoop(std::bind(&TcpConnection::SendInLoop, shared_from_this(), data, len));
}

void TcpConnection::SendInLoop(const void * data, size_t len)
{
    loop_->AssertInLoopThread();
    ssize_t nwrote = 0;
    size_t remain = len;
    bool fatalError = false;

    if(state_ == DISCONNECTED)
    {
        LOG_WARNING << "Disconnected, give up writing";
        return;
    }

    if(!channel_->IsWriting() && outputBuffer_.ReadableBytes() == 0)
    {
        nwrote = sockets::Write(channel_->GetFd(), data, len);
        if(nwrote >= 0)
        {
            remain = len - static_cast<size_t>(nwrote);
            if(remain == 0 && writeCompleteCallback_)
            {
                loop_->RunInLoop(std::bind(writeCompleteCallback_, shared_from_this()));
            }
        }
        else
        {
            nwrote = 0;
            if(errno != EWOULDBLOCK)
            {
                LOG_ERROR << "TcpConnection::SendInLoop";
                if(errno == EPIPE || errno == ECONNRESET)
                {
                    fatalError = true;
                }
            }
        }
    }

    assert(remain <= len);
    if(!fatalError && remain > 0)
    {
        outputBuffer_.Append(static_cast<const char *>(data) + nwrote, remain);
        if(!channel_->IsWriting())
        {
            channel_->EnableWriting();
        }
    }
}

void TcpConnection::Shutdown()
{
    if(state_ == CONNECTED)
    {
        state_ = DISCONNECTING;
        loop_->RunInLoop(std::bind(&TcpConnection::ShutdownInLoop, shared_from_this()));
    }
}

void TcpConnection::ShutdownInLoop()
{
    loop_->AssertInLoopThread();
    if(!channel_->IsWriting())
    {
        socket_->ShutdownWrite();
    }
}

void TcpConnection::HandleRead()
{
    loop_->AssertInLoopThread();
    auto savedError = 0;
    auto n = inputBuffer_.ReadFd(channel_->GetFd(), &savedError);
    if(n > 0)
    {
        messageCallback_(shared_from_this(), inputBuffer_);
    }
    else
    {
        if(n == 0)
        {
            HandleClose();
        }
        else
        {
            errno = savedError;
            LOG_ERROR << "TcpConnection::HandleRead()";
            HandleError();
        }
    }
}

void TcpConnection::HandleClose()
{
    loop_->AssertInLoopThread();
    assert(state_ == CONNECTED || state_ == DISCONNECTING);
    state_ = DISCONNECTED;
    channel_->DisableAll();

    auto guardThis = shared_from_this();
    connectionCallback_(guardThis);
    closeCallback_(guardThis);
}

void TcpConnection::HandleWrite()
{
    loop_->AssertInLoopThread();
    if(channel_->IsWriting())
    {
        auto n = sockets::Write(channel_->GetFd(), outputBuffer_.BeginRead(), outputBuffer_.ReadableBytes());
        if(n > 0)
        {
            outputBuffer_.Retrieve(static_cast<size_t>(n));
            if(outputBuffer_.ReadableBytes() == 0)
            {
                channel_->DisableWriting();

                if(state_ == DISCONNECTING)
                {
                    //asset in loop thread already
                    ShutdownInLoop();
                }
            }
        }
        else
        {
            LOG_ERROR << "TcpConnection::HandleWrite()";
        }
    }
    else
    {
        LOG_INFO << "Connection fd = " << channel_->GetFd() << " is down, no more writing";
    }
}

void TcpConnection::HandleError()
{
    char buf[64];
    auto error = sockets::GetError(channel_->GetFd());
    LOG_ERROR << "TcpConnection::HandleError(): [" << name_ << "] - SO_ERROR = " << error << strerror_r(error, buf, sizeof(buf));
}

void TcpConnection::SetTcpNoDelay(bool on)
{
    socket_->SetTcpNoDelay(on);
}

void TcpConnection::SetKeepAlive(bool on)
{
    socket_->SetKeepAlive(on);
}