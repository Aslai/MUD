#ifndef MUD_GLOBAL_MUTEX_HPP
#define MUD_GLOBAL_MUTEX_HPP


#ifdef _WIN32
    #include <windows.h>
#else
    #include <pthread.h>
#endif
#include "Global/RefCounter.hpp"

namespace GlobalMUD{
    class Mutex{
    #ifdef _WIN32
        RefCounter<HANDLE> mutex;
    #else
        RefCounter<pthread_mutex_t> mutex;
    #endif

	public:
		Mutex();
		//Mutex( Mutex& other );
		Mutex& operator=( Mutex& other );
		~Mutex();
		void Lock();
		void Unlock();
		void Wait();
	};
}

#endif
