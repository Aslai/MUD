#include "Thread/Mutex.hpp"
#include <string>
#include <vector>
#include<cstdio>
namespace GlobalMUD{
#ifdef _WIN32
int NUM = 0;
        Mutex::Inner::Inner(){
            mutex = CreateMutex( NULL, FALSE, NULL);
        }
        Mutex::Inner::~Inner(){
            CloseHandle( mutex );
        }
        HANDLE Mutex::Inner::operator*(){
            return mutex;
        }

        Mutex::Mutex() : mutex( new Mutex::Inner() )
        {

        }

		Mutex& Mutex::operator=( Mutex& other ){
            //Take a reference to the other mutex's handle
            mutex = other.mutex;
            return *this;
		}

		Mutex::Mutex( const Mutex &other ){
            //Take a reference to the other mutex's handle
            mutex = other.mutex;
		}

        Mutex::~Mutex()
        {

        }

        void Mutex::Lock()
        {
            WaitForSingleObject( mutex->mutex, INFINITE);
        }

        void Mutex::Unlock()
        {
            ReleaseMutex(mutex->mutex);
        }
#else
        Mutex::Inner::Inner(){
            pthread_mutex_init(&mutex,0);
        }
        Mutex::Inner::~Inner(){
            pthread_mutex_destroy( &mutex );
        }
        pthread_mutex_t Mutex::Inner::operator*(){
            return mutex;
        }

		Mutex::Mutex() : mutex( new Mutex::Inner() )
		{

		}

		Mutex& Mutex::operator=( Mutex& other ){
            //Take a reference to the other mutex's handle
            mutex = other.mutex;
            return *this;
		}

		Mutex::~Mutex()
		{

		}

		void Mutex::Lock()
		{
			pthread_mutex_lock( mutex->mutex );
		}

		void Mutex::Unlock()
		{
			pthread_mutex_unlock( mutex->mutex );
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
