#ifndef MUD_GLOBAL_THREAD_HPP
#define MUD_GLOBAL_THREAD_HPP


#ifdef _WIN32
    #include <windows.h>
    #define THREADCallFunction __attribute__((__stdcall__))
    #define THREADRETURN long unsigned int
#else
    #include <pthread.h>
    #define THREADCallFunction
    #define THREADRETURN void*
#endif
#include <vector>
#include <map>
#include "Global/Mutex.hpp"
#include <functional>

namespace GlobalMUD{
    class Thread{
        Mutex Lock;
        bool valid;
        struct Arguments{
            std::function<void()> f;
            Mutex Lock;
        };

        static THREADRETURN THREADCallFunction ThreadFunc(void*d);

        #ifdef _WIN32
        HANDLE ThreadHandle;
        DWORD ThreadID;
        #else
        pthread_t ThreadHandle;
        int ThreadID;
        #endif
        bool detached;
    public:
        Thread(std::function<void()> f);
        ~Thread();
        void Run();
        void Join();
        void Suspend();
        void Kill();
        void Detach();
        static void Sleep(unsigned long msec);
        #ifdef RunUnitTests
        static bool RunTests();
        #endif

    };
}

#endif
