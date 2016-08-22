#include <SummerNet/net/Buffer.h>
#include <SummerNet/net/SocketOps.h>

#include <errno.h>
#include <unistd.h>
#include <algorithm>

using namespace summer;
using namespace summer::net;

ssize_t Buffer::ReadFd(int fd, int * savedError)
{
    char extraBuf[65536];
    struct iovec vec[2];
    auto writable = WritableBytes();
    vec[0].iov_base = BeginWrite();
    vec[0].iov_len = writable;
    vec[1].iov_base = extraBuf;
    vec[1].iov_len = sizeof(extraBuf);

    auto iovCnt = (writable < sizeof(extraBuf)? 2: 1);
    auto n = sockets::ReadV(fd, vec, iovCnt);
    if(n < 0)
    {
        *savedError = errno;
    }
    else
    {
        if(static_cast<size_t>(n) <= writable)
        {
            writeIndex_ += static_cast<size_t>(n);
        }
        else
        {
            writeIndex_ = buffer_.size();
            Append(extraBuf, static_cast<size_t>(n) - writable);
        }
    }

    return n;
}

void Buffer::MakeSpace(size_t len)
{
    if(WritableBytes() + PrependableBytes() < len)
    {
        buffer_.resize(writeIndex_ + len);
    }
    else
    {
        assert(prepSize < readIndex_);
        auto readable = ReadableBytes();
        std::copy(buffer_.data() + readIndex_, buffer_.data() + writeIndex_, buffer_.data() + prepSize);
        readIndex_ = prepSize;
        writeIndex_ = readIndex_ + readable;
        assert(readable = ReadableBytes());
    }
}

//const char CRLF[] = "\n\r";