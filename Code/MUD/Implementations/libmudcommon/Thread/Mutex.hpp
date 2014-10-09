#ifndef MUD_THREAD_MUTEX_HPP
#define MUD_THREAD_MUTEX_HPP

#include "libmudcommon/dllinfo.hpp"

#ifdef _WIN32
    #include <windows.h>
    #undef LoadString
#else
    #include <pthread.h>
#endif
#include "libmudcommon/Memory/RefCounter.hpp"

namespace MUD{
    class Mutex{
        class Inner{
            public:
            #ifdef _WIN32
            HANDLE mutex;
            HANDLE operator*();
            #else
            pthread_mutexattr_t attr;
            pthread_mutex_t mutex;
            pthread_mutex_t operator*();
            #endif
            DLL_EXPORT Inner();
            DLL_EXPORT ~Inner();
        };
        RefCounter<Inner> mutex;
	public:
		DLL_EXPORT Mutex();
		DLL_EXPORT Mutex( const Mutex &other );
		DLL_EXPORT Mutex& operator=( Mutex& other );
		DLL_EXPORT ~Mutex();
		DLL_EXPORT void Lock();
		DLL_EXPORT void Unlock();
		DLL_EXPORT void Wait();
	};
    class ScopedMutex{
        Mutex m;
    public:
        DLL_EXPORT ScopedMutex(Mutex M);
		DLL_EXPORT ~ScopedMutex();
		DLL_EXPORT void Lock();
		DLL_EXPORT void Unlock();
		DLL_EXPORT void Wait();
    };
}

#endif
