#include "Global/Mutex.hpp"
#include <string>
#include <vector>

namespace GlobalMUD{
#ifdef _WIN32
        Mutex::Mutex() : mutex( new HANDLE )
        {
            *mutex = CreateMutex( NULL, FALSE, NULL);
        }

		Mutex& Mutex::operator=( Mutex& other ){
            if( mutex.References() <= 1 )
                CloseHandle( *mutex );
            mutex = other.mutex;
            return *this;
		}

        Mutex::~Mutex()
        {
            if( mutex.References() <= 1 )
                CloseHandle( *mutex );
        }

        void Mutex::Lock()
        {
            WaitForSingleObject( *mutex, INFINITE);
        }

        void Mutex::Unlock()
        {
            ReleaseMutex(*mutex);
        }
#else
		Mutex::Mutex() : mutex( new pthread_mutex_t )
		{
            pthread_mutex_init(mutex.get(),0);
		}

		Mutex& Mutex::operator=( Mutex& other ){
            if( mutex.References() <= 1 )
                pthread_mutex_destroy( mutex.get() );
            mutex = other.mutex;
            return *this;
		}

		Mutex::~Mutex()
		{
		    if( mutex.References() <= 1 )
                pthread_mutex_destroy( mutex.get() );
		}

		void Mutex::Lock()
		{
			pthread_mutex_lock( mutex.get() );
		}

		void Mutex::Unlock()
		{
			pthread_mutex_unlock( mutex.get() );
		}
#endif

		void Mutex::Wait()
		{
			Lock();
			Unlock();
		}
}
