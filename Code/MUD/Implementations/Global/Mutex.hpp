#include <windows.h>
#include "Global/RefCounter.hpp"

namespace GlobalMUD{
    class Mutex{
		RefCounter<HANDLE> mutex;
	public:
		Mutex();
		~Mutex();
		void Lock();
		void Unlock();
		void Wait();
	};
}
