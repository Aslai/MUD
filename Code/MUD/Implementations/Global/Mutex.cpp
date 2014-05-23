#include "Global/Mutex.hpp"
#include <string>
#include <vector>

namespace GlobalMUD{

		Mutex::Mutex() : mutex( new pthread_mutex_t )
		{

			*mutex = CreateMutex( NULL, FALSE, NULL);
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

		void Mutex::Wait()
		{
			WaitForSingleObject( *mutex, INFINITE);
			ReleaseMutex(*mutex);
		}
}
