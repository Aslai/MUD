#include "Thread/Mutex.hpp"
#include <string>
#include <vector>

namespace GlobalMUD{
#ifdef _WIN32
        Mutex::Mutex() : mutex( new HANDLE )
        {
            *mutex = CreateMutex( NULL, FALSE, NULL);
        }

		Mutex& Mutex::operator=( Mutex& other ){
		    //If this is the last copy of the mutex handle, close it.
            if( mutex.References() <= 1 )
                CloseHandle( *mutex );
            //Take a reference to the other mutex's handle
            mutex = other.mutex;
            return *this;
		}

		Mutex::Mutex( const Mutex &other ){
		    //If this is the last copy of the mutex handle, close it.
            if( mutex.References() <= 1 )
                CloseHandle( *mutex );
            //Take a reference to the other mutex's handle
            mutex = other.mutex;
		}

        Mutex::~Mutex()
        {
            //If this is the last copy of the mutex handle, close it.
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
		    //If this is the last copy of the mutex handle, close it.
            if( mutex.References() <= 1 )
                pthread_mutex_destroy( mutex.get() );
            //Take a reference to the other mutex's handle
            mutex = other.mutex;
            return *this;
		}

		Mutex::~Mutex()
		{
		    //If this is the last copy of the mutex handle, close it.
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
            //Wait for the mutex to be released.
			Lock();
			Unlock();
		}


        ScopedMutex::ScopedMutex(Mutex M){
            m = M;
		}

		ScopedMutex::~ScopedMutex(){
            m.Unlock();
		}

		void ScopedMutex::Lock(){
            m.Lock();
		}

		void ScopedMutex::Unlock(){
            m.Unlock();
		}

		void ScopedMutex::Wait(){
            m.Wait();
		}
}
