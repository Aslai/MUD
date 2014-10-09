#include "Thread/Mutex.hpp"
#include <string>
#include <vector>
#include<cstdio>
namespace MUD{
#ifdef _WIN32
        DLL_EXPORT Mutex::Inner::Inner(){
            mutex = CreateMutex( nullptr, FALSE, nullptr);
        }
        DLL_EXPORT Mutex::Inner::~Inner(){
            CloseHandle( mutex );
        }
        DLL_EXPORT HANDLE Mutex::Inner::operator*(){
            return mutex;
        }

        DLL_EXPORT Mutex::Mutex() : mutex( new Mutex::Inner() )
        {

        }

		DLL_EXPORT Mutex& Mutex::operator=( Mutex& other ){
            //Take a reference to the other mutex's handle
            mutex = other.mutex;
            return *this;
		}

		DLL_EXPORT Mutex::Mutex( const Mutex &other ){
            //Take a reference to the other mutex's handle
            mutex = other.mutex;
		}

        DLL_EXPORT Mutex::~Mutex()
        {

        }

        DLL_EXPORT void Mutex::Lock()
        {
            WaitForSingleObject( mutex->mutex, INFINITE);
        }

        DLL_EXPORT void Mutex::Unlock()
        {
            ReleaseMutex(mutex->mutex);
        }
#else
        DLL_EXPORT Mutex::Inner::Inner(){
            pthread_mutexattr_init( &attr );
            pthread_mutexattr_settype( &attr, PTHREAD_MUTEX_RECURSIVE );
            pthread_mutex_init(&mutex, &attr );
        }
        DLL_EXPORT Mutex::Inner::~Inner(){
            pthread_mutex_destroy( &mutex );
            pthread_mutexattr_destroy( &attr );
        }
        DLL_EXPORT pthread_mutex_t Mutex::Inner::operator*(){
            return mutex;
        }

		DLL_EXPORT Mutex::Mutex() : mutex( new Mutex::Inner() )
		{

		}
        DLL_EXPORT Mutex::Mutex( const Mutex &other ){
            //Take a reference to the other mutex's handle
            mutex = other.mutex;
		}

		DLL_EXPORT Mutex& Mutex::operator=( Mutex& other ){
            //Take a reference to the other mutex's handle
            mutex = other.mutex;
            return *this;
		}

		DLL_EXPORT Mutex::~Mutex()
		{

		}

		DLL_EXPORT void Mutex::Lock()
		{
			pthread_mutex_lock( &mutex->mutex );
		}

		DLL_EXPORT void Mutex::Unlock()
		{
			pthread_mutex_unlock( &mutex->mutex );
		}
#endif

		DLL_EXPORT void Mutex::Wait()
		{
            //Wait for the mutex to be released.
			Lock();
			Unlock();
		}


        DLL_EXPORT ScopedMutex::ScopedMutex(Mutex M){
            m = M;
		}

		DLL_EXPORT ScopedMutex::~ScopedMutex(){
            m.Unlock();
		}

		DLL_EXPORT void ScopedMutex::Lock(){
            m.Lock();
		}

		DLL_EXPORT void ScopedMutex::Unlock(){
            m.Unlock();
		}

		DLL_EXPORT void ScopedMutex::Wait(){
            m.Wait();
		}
}
