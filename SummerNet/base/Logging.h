#ifndef SUMMER_NET_LOGGING
#define SUMMER_NET_LOGGING

#include <glog/logging.h>

namespace summer
{
    enum class LogLevel
    {
        INFO = 0,
        WARNING,
        ERROR,
        FATAL,
    };

    extern LogLevel gLogLevel;

    inline LogLevel GetLogLevel()
    {
        return gLogLevel;
    }
}

#define LOG_INFO if(summer::GetLogLevel() <= summer::LogLevel::INFO) LOG(INFO)
#define LOG_WARNING if(summer::GetLogLevel() <= summer::LogLevel::WARNING) LOG(WARNING)
#define LOG_ERROR if(summer::GetLogLevel() <= summer::LogLevel::ERROR) LOG(ERROR)
#define LOG_FATAL if(summer::GetLogLevel() <= summer::LogLevel::FATAL) LOG(FATAL)
#endif
