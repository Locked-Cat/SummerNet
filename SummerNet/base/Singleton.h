#ifndef SUMMER_NET_SINGLETON
#define SUMMER_NET_SINGLETON

#include <boost/noncopyable.hpp>

#include <mutex>
#include <cstdlib>

namespace summer
{
    template <typename T>
    class Singleton
        : boost::noncopyable
    {
    public:
        static T & Instance()
        {
            std::call_once(flag_, &Singleton::Init);
            return *value_;
        }
    private:
        Singleton();
        ~Singleton();

        static void Init()
        {
            value_ = new T();
            ::atexit(&Singleton::Destroy);
        }

        static void Destroy()
        {
            delete value_;
            value_ = nullptr;
        }
        
        static std::once_flag flag_;
        static T * value_;
    };

    template <typename T>
    std::once_flag Singleton<T>::flag_;

    template <typename T>
    T * Singleton<T>::value_ = nullptr;
}

#endif
