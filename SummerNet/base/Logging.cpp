#include <SummerNet/base/Singleton.h>
#include <SummerNet/base/Logging.h>

#include <cstdlib>
#include <sys/stat.h>

namespace summer
{
    LogLevel InitLogLevel()
    {
        auto logLevel = LogLevel::INFO;
        if(std::getenv("SUMMER_NET_LOG_WARNING"))
        {
            logLevel = LogLevel::WARNING;
        }
        else
        {
            if(std::getenv("SUMMER_NET_LOG_ERROR"))
            {
                logLevel =  LogLevel::ERROR;
            }
            else
            {
                if(std::getenv("SUMMER_NET_LOG_FATAL"))
                {
                    logLevel = LogLevel::FATAL;
                }
            }
        }  

        ::mkdir("./log", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        FLAGS_minloglevel = int(logLevel);
        FLAGS_logtostderr = false;
        FLAGS_alsologtostderr = false;
        FLAGS_log_dir = "./log";
        google::InitGoogleLogging("sumnet");
        std::atexit(&google::ShutdownGoogleLogging);

        return logLevel;
    } 

    LogLevel gLogLevel = InitLogLevel();
}