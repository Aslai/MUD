#ifndef MUD_THREAD_MUTEX_HPP
#define MUD_THREAD_MUTEX_HPP


#ifdef _WIN32
    #include <windows.h>
#else
    #include <pthread.h>
#endif
#include "Memory/RefCounter.hpp"

namespace GlobalMUD{
    class Mutex{
    #ifdef _WIN32
        RefCounter<HANDLE> mutex;
    #else
        RefCounter<pthread_mutex_t> mutex;
    #endif

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
