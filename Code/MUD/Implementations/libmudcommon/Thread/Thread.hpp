#ifndef MUD_THREAD_THREAD_HPP
#define MUD_THREAD_THREAD_HPP

#include "libmudcommon/dllinfo.hpp"

#ifdef _WIN32
    #include <windows.h>
    #define THREADCallFunction __attribute__((__stdcall__))
    #define THREADRETURN long unsigned int
    #undef LoadString
#else
    #include <pthread.h>
    #define THREADCallFunction
    #define THREADRETURN void*
#endif
#include <vector>
#include <map>
#include "libmudcommon/Thread/Mutex.hpp"
#include <functional>

namespace MUD{
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
        DLL_EXPORT Thread(std::function<void()> f);
        DLL_EXPORT ~Thread();
        DLL_EXPORT void Run();
        DLL_EXPORT void Join();
        DLL_EXPORT void Suspend();
        DLL_EXPORT void Kill();
        DLL_EXPORT void Detach();
        DLL_EXPORT static void Sleep(unsigned long msec);
        #ifdef RunUnitTests
        DLL_EXPORT static bool RunTests();
        #endif

    };
}

#endif
