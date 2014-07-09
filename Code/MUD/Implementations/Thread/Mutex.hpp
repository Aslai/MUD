#ifndef MUD_THREAD_MUTEX_HPP
#define MUD_THREAD_MUTEX_HPP


#ifdef _WIN32
    #include <windows.h>
    #undef LoadString
#else
    #include <pthread.h>
#endif
#include "Memory/RefCounter.hpp"

namespace GlobalMUD{
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
            Inner();
            ~Inner();
        };
        RefCounter<Inner> mutex;
	public:
		Mutex();
		Mutex( const Mutex &other );
		Mutex& operator=( Mutex& other );
		~Mutex();
		void Lock();
		void Unlock();
		void Wait();
	};
    class ScopedMutex{
        Mutex m;
    public:
        ScopedMutex(Mutex M);
		~ScopedMutex();
		void Lock();
		void Unlock();
		void Wait();
    };
}

#endif
