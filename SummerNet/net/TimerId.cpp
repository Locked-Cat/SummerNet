#include <SummerNet/net/TimerId.h>

using namespace summer::net;

TimerId::TimerId()
    : id_(IncrementSequenceAndGet())
{
}

uint64_t TimerId::IncrementSequenceAndGet()
{
    auto res = sequence_.fetch_add(1);
    return res;
}

std::atomic<uint64_t> TimerId::sequence_(0);