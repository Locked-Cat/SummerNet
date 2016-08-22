#ifndef SUMMER_NET_BUFFER
#define SUMMER_NET_BUFFER

#include <vector>
#include <string>
#include <algorithm>
#include <cassert>
#include <cstring>

namespace summer
{
    namespace net
    {
        class Buffer
        {
        public:
            static const size_t prepSize = 8;
            static const size_t initSize = 1024;

            Buffer(size_t size = initSize)
                : buffer_(size + prepSize)
                , readIndex_(prepSize)
                , writeIndex_(prepSize)
            {
            }

            void Swap(Buffer & rhs)
            {
                buffer_.swap(rhs.buffer_);
                std::swap(readIndex_, rhs.readIndex_);
                std::swap(writeIndex_, rhs.writeIndex_);
            }

            size_t ReadableBytes() const
            {
                return writeIndex_ - readIndex_;
            }

            size_t WritableBytes() const
            {
                return buffer_.size() - writeIndex_;
            }

            size_t PrependableBytes() const
            {
                return readIndex_;
            }

            char * Begin()
            {
                return buffer_.data();
            }

            const char * Begin() const
            {
                return buffer_.data();
            }

            char * BeginWrite()
            {
                return Begin() + writeIndex_;
            }

            const char * BeginRead()
            {
                return Begin() + readIndex_;
            }

            void Append(const void * data, size_t len)
            {
                if(WritableBytes() < len)
                {
                    MakeSpace(len);
                }
                assert(WritableBytes() >= len);
                ::memcpy(BeginWrite(), data, len);
                writeIndex_ += len;
            }

            const char * Peek()
            {
                return Begin() + readIndex_;
            }

            void Retrieve(size_t len)
            {
                assert(len <= ReadableBytes());
                if(len < ReadableBytes())
                {
                    readIndex_ += len;
                }
                else
                {
                    RetrieveAll();
                }
            }

            void RetrieveAll()
            {
                readIndex_ = prepSize;
                writeIndex_ = prepSize;
            }

            std::string RetrieveAsString(size_t len)
            {
                assert(len <= ReadableBytes());
                std::string result(Peek(), len);
                Retrieve(len);
                return result;
            }

            std::string RetrieveAllAsString()
            {
                return RetrieveAsString(ReadableBytes());
            }

            ssize_t ReadFd(int fd, int * savedError);
        private:
            std::vector<char> buffer_;
            size_t readIndex_;
            size_t writeIndex_;

            void MakeSpace(size_t len);

            //static char CRLF[];
        };
    }
}

#endif