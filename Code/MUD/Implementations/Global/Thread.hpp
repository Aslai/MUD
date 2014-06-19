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

namespace GlobalMUD{
    namespace Private{
        template<class... T>
        struct StorageInternal;

        template<class T1, class... T2>
        struct StorageInternal<T1,T2...>{
            T1 val;
            StorageInternal<T2...> next;
            static constexpr int type = ((sizeof...(T2))==0)?2:1;
            StorageInternal(T1 v, T2... args) : next(args...){
                val = v;
            }
        };
        template<>
        struct StorageInternal<>{
            static constexpr int type = 3;
            StorageInternal(){
            }
        };
        template<class Function, class... arguments>
        struct Storage{
            Function f;
            Mutex Lock;
            StorageInternal<arguments...> vals;
            Storage(Function ff, arguments... args) : vals(args...){
                f = ff;
            }
        };
        template< bool Conditional, class THEN, class ELSE > struct selector;

        template <class THEN, class ELSE>
        struct selector< true, THEN, ELSE >
        {typedef THEN SELECT_CLASS;};

        template <class THEN, class ELSE>
        struct selector< false, THEN, ELSE >
        {typedef ELSE SELECT_CLASS;};

        template< bool Condition, class THEN, class ELSE > struct IF
        {
            typedef typename selector< Condition, THEN, ELSE >::SELECT_CLASS RESULT;
        };

        struct ELSE
        {
            template<class Function, class... Arguments, class... Args2>
            static void func(Function f, StorageInternal<Arguments...> s, Args2... args)
            {
                //If you are seeing an error here, you are probably passing in an incorrect number of arguments to the function.
                f(args...);
            }
        };

        struct THEN
        {
            template<class Function, class... Arguments, class... Args2>
            static void func(Function f, StorageInternal<Arguments...> s, Args2... args)
            {
                //If you are seeing an error here, you are probably passing in an incorrect number of arguments to the function.
                IF< s.type == 1, THEN, ELSE>::RESULT::func(f, s.next, args..., s.val );
            }
        };

        template<class Function, class... T>
        struct CallFunction{
            CallFunction( Storage<Function, T...> s ){
                IF< s.vals.type < 3, THEN, ELSE>::RESULT::func(s.f, s.vals);
            }
        };
    }

    class Thread{
        Mutex Lock;
        bool valid;
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

        template<class Function, class... Arguments>
        static THREADRETURN THREADCallFunction ThreadFunc(void*d){

            Private::Storage<Function, Arguments...> *store = (Private::Storage<Function, Arguments...>*)d;
            #ifndef _WIN32
            store->Lock.Wait();
            #endif
            Private::CallFunction<Function, Arguments...>a(*store);

            delete store;
            return 0;
        }


        #ifdef _WIN32
        HANDLE ThreadHandle;
        DWORD ThreadID;
        #else
        pthread_t ThreadHandle;
        int ThreadID;
        #endif

        struct mem{
            int type;
            void* ptr;
        };
        std::map<void*, mem> allocations;
        bool detached;
    public:
        template<class Function, class... Argument>
        Thread(Function f, Argument... arg){
            detached = false;
            Private::Storage<Function, Argument...> *store = new Private::Storage<Function, Argument...>(f, arg...);

            store->Lock.Lock();
            Lock = store->Lock;
            #ifdef _WIN32
            ThreadHandle = CreateThread( NULL, 2048, GlobalMUD::Thread::ThreadFunc<Function, Argument...>, store, CREATE_SUSPENDED, &ThreadID );
            valid = ThreadHandle != NULL;
            #else
            pthread_attr_t attr;
            pthread_attr_init( &attr );
            valid = 0 == pthread_create(&ThreadHandle, NULL, GlobalMUD::Thread::ThreadFunc<Function, Argument...>, store);

            pthread_attr_destroy( &attr );
            #endif
        }
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
    class ThreadMember : public Thread{
        public:
        template<class T, class C, class... Argument>
        ThreadMember(T(C::*f)(Argument...), C* object, Argument... arg) : Thread(Functor<T, C, Argument...>(f, object), arg...){
        }
    };
}

#endif
