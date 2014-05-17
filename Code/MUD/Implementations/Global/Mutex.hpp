#include <windows.h>

namespace GlobalMUD{
    class Mutex{
		HANDLE mutex;
	public:
		Mutex();
		~Mutex();
		void Lock();
		void Unlock();
		void Wait();
	};
}
