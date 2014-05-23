#ifdef _WIN32
    #include <windows.h>
    #define THREADCALL __attribute__((__stdcall__))
#else
    #include <pthread.h>
    #define THREADCALL
#endif
#include <vector>
#include <map>
#include "Global/Mutex.hpp"

namespace GlobalMUD{


    class Thread{
        template<class T, class C, class... Argument>
        class Functor{
            C* obj;
            T(C::*func)(Argument...);
            public:
            Functor(T(C::*function)(Argument...), C* object){
                obj = object;
                func = function;
            }
            Functor(){
                obj = NULL;
            }
            T operator()(Argument... args){
                return ((obj)->*(func))(args...);
            }
        };
        template<class Function, class Argument0 = int, class Argument1 = int, class Argument2 = int, class Argument3 = int, class Argument4 = int>
        struct storage{
            Mutex Lock;
            Function f;
            Argument0 a0;
            Argument1 a1;
            Argument2 a2;
            Argument3 a3;
            Argument4 a4;
            storage(Function F, Argument0 A0 = Argument0(), Argument1 A1 = Argument1(), Argument2 A2 = Argument2(), Argument3 A3 = Argument3(), Argument4 A4 = Argument4() ){
                f = F;
                a0 = A0;
                a1 = A1;
                a2 = A2;
                a3 = A3;
                a4 = A4;
            }
        };
        template<class Function>
        static long unsigned int THREADCALL ThreadFunc(void*d){
            storage<Function, int, int, int, int, int> *store = (storage<Function, int, int, int, int, int> *)d;
            #ifndef _WIN32
            store->Lock.Wait();
            #endif
            store->f();
            delete store;
            #ifndef _WIN32
            pthread_detach(pthread_self());
            #endif
            return 0;
        }
        template<class Function, class Argument0>
        static long unsigned int THREADCALL ThreadFunc(void*d){
            storage<Function, Argument0, int, int, int, int> *store = (storage<Function, Argument0, int, int, int, int> *)d;
            #ifndef _WIN32
            store->Lock.Wait();
            #endif
            store->f(store->a0);
            delete store;
            #ifndef _WIN32
            pthread_detach(pthread_self());
            #endif
            return 0;
        }
        template<class Function, class Argument0, class Argument1>
        static long unsigned int THREADCALL ThreadFunc(void*d){
            storage<Function, Argument0, Argument1, int, int, int> *store = (storage<Function, Argument0, Argument1, int, int, int> *)d;
            #ifndef _WIN32
            store->Lock.Wait();
            #endif
            store->f(store->a0, store->a1);
            delete store;
            #ifndef _WIN32
            pthread_detach(pthread_self());
            #endif
            return 0;
        }
        template<class Function, class Argument0, class Argument1, class Argument2>
        static long unsigned int THREADCALL ThreadFunc(void*d){
            storage<Function, Argument0, Argument1, Argument2, int, int> *store = (storage<Function, Argument0, Argument1, Argument2, int, int> *)d;
            #ifndef _WIN32
            store->Lock.Wait();
            #endif
            store->f(store->a0, store->a1, store->a2);
            delete store;
            #ifndef _WIN32
            pthread_detach(pthread_self());
            #endif
            return 0;
        }
        template<class Function, class Argument0, class Argument1, class Argument2, class Argument3>
        static long unsigned int THREADCALL ThreadFunc(void*d){
            storage<Function, Argument0, Argument1, Argument2, Argument3, int> *store = (storage<Function, Argument0, Argument1, Argument2, Argument3, int> *)d;
            #ifndef _WIN32
            store->Lock.Wait();
            #endif
            store->f(store->a0, store->a1, store->a2, store->a3);
            delete store;
            #ifndef _WIN32
            pthread_detach(pthread_self());
            #endif
            return 0;
        }
        template<class Function, class Argument0, class Argument1, class Argument2, class Argument3, class Argument4>
        static long unsigned int THREADCALL ThreadFunc(void*d){
            storage<Function, Argument0, Argument1, Argument2, Argument3, Argument4> *store = (storage<Function, Argument0, Argument1, Argument2, Argument3, Argument4> *)d;
            #ifndef _WIN32
            store->Lock.Wait();
            #endif
            store->f(store->a0, store->a1, store->a2, store->a3, store->a4);
            delete store;
            #ifndef _WIN32
            pthread_detach(pthread_self());
            #endif
            return 0;
        }
        #ifdef _WIN32
        HANDLE ThreadHandle;
        DWORD ThreadID;
        #else
        int ThreadHandle;
        pthread_t ThreadID;
        #endif

        struct mem{
            int type;
            void* ptr;
        };
        std::map<void*, mem> allocations;
    public:
        template<class Function, class... Argument>
        Thread(Function f, Argument... arg){
            //!If you see an error here, that means you probably passed in too many parameters. Implement ThreadFunc for your number of parameters.
            storage<Function, Argument...> *store = new storage<Function, Argument...>(f, arg...);
            store->Lock.Lock();
            #ifdef _WIN32
            ThreadHandle = CreateThread( NULL, 2048, GlobalMUD::Thread::ThreadFunc<Function, Argument...>, store, CREATE_SUSPENDED, &ThreadID );
            #else
            pthread_attr_t attr;
            pthread_attr_init( &attr );
            ThreadHandle = pthread_create(&ThreadID, &attr, (void*(*)(void*))GlobalMUD::Thread::ThreadFunc<Function, Argument...>, store);
            pthread_attr_destroy( &attr );
            #endif
            //ThreadHandle = NULL;
            //GlobalMUD::Thread::ThreadFunc<Function, Argument...>(store);
        }
        //template<class T, class C, class... Argument>
        //Thread(T(C::*f)(Argument...), C* object, Argument... arg) : Thread(Functor<T, C, Argument...>(f, object), arg...){
        //}
        ~Thread();
        void Run();
        void Join();
        void Suspend();
        void Kill();
        static void Sleep(unsigned long msec);
        #ifdef RunUnitTests
        static bool RunTests();
        #endif

    };
    class ThreadMember : public Thread{
        public:
        template<class T, class C, class... Argument>
        ThreadMember(T(C::*f)(Argument...), C* object, Argument... arg) : Thread(Functor<T, C, Argument...>(f, object), arg...){
        }
    };
}
