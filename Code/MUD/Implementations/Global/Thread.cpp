#include "Global/Thread.hpp"
#ifdef _WIN32
    #include <windows.h>
#else
    #include <pthread.h>
    #include <unistd.h>
#endif
#include "Global/Mutex.hpp"
#include "Global/Error.hpp"
#include <cstdio>

namespace GlobalMUD{
    Thread::~Thread(){
        Run();
        Join();
    }
    #ifdef _WIN32
    void Thread::Run(){
        ResumeThread( ThreadHandle );
    }

    void Thread::Join(){
        WaitForSingleObject( ThreadHandle, INFINITE );
    }

    void Thread::Suspend(){
        SuspendThread( ThreadHandle );
    }

    void Thread::Kill(){
        TerminateThread( ThreadHandle, 0 );
    }

    void Thread::Sleep(unsigned long msec){
        ::Sleep(msec);
    }
    #else
    void Thread::Run(){
        Lock.Unlock();
    }

    void Thread::Join(){
        if( valid )
            pthread_join( ThreadHandle, NULL );
        valid = !valid;
    }

    void Thread::Suspend(){
    //No-op with pthreads
    }

    void Thread::Kill(){
        if( valid )
            pthread_cancel( ThreadHandle );
    }

    void Thread::Sleep(unsigned long msec){
        ::usleep(msec*1000);
    }
    #endif

    #ifdef RunUnitTests
    struct functions{
        static int accumulator;
        int accumulator2;
        static void testA(){
            for( int i = 0; i < 100000; ++i ){
                accumulator++;
            }
        }
        static void testB_1(){
            for( int i = 0; i < 100000; ++i ){
                accumulator--;
            }
        }
        static void testB_2(){
            for( int i = 0; i < 100000; ++i ){
                accumulator*=2;
            }
        }
        static void testC_1(Mutex m){
            accumulator = 1;
            m.Lock();
            for( int i = 0; i < 24; ++i ){
                accumulator*=2;
            }
            m.Unlock();
        }
        static void testC_2(Mutex m){
            m.Lock();
            for( int i = 0; i < 100000; ++i ){
                accumulator-=1;
            }
            m.Unlock();
        }
        static void testD_1(){
            accumulator = 0;
        }
        static void testD_2(int a){
            accumulator += a;
        }
        static void testD_3(int a, int b ){
            accumulator += a + b;
        }
        static void testD_4(int a, int b, int c ){
            accumulator += a + b + c;
        }
        static void testD_5(int a, int b, int c, int d ){
            accumulator += a + b + c + d;
        }
        static void testD_6(int a, int b, int c, int d, int e ){
            accumulator += a + b + c + d + e;
        }
        void operator()(int a){
            accumulator = a;
        }
        void testF(int a, int b){
            accumulator2 = a + b;
        }

    };
    int functions::accumulator;
    bool Thread::RunTests(){
        TEST("GlobalMUD::Thread");

        Thread testA(functions::testA);
        testA.Run();
        testA.Join();
        ASSERT(functions::accumulator == 100000);


        Thread testB_1(functions::testB_1);
        Thread testB_2(functions::testB_2);
        testB_1.Run();
        testB_1.Join();
        testB_2.Run();
        testB_2.Join();
        ASSERT( functions::accumulator == 0 );

        Mutex testCm;
        testCm.Lock();
        Thread testC_1(functions::testC_1, testCm);
        Thread testC_2(functions::testC_2, testCm);
        testC_1.Run();
        testCm.Unlock();
        Thread::Sleep(100);
        testCm.Lock();
        testC_2.Run();
        testCm.Unlock();
        testC_1.Join();
        testC_2.Join();
        ASSERT( functions::accumulator == (16777216-100000) );

        Thread testD_1(functions::testD_1);
        Thread testD_2(functions::testD_2,1);
        Thread testD_3(functions::testD_3,2,3);
        Thread testD_4(functions::testD_4,4,5,6);
        Thread testD_5(functions::testD_5,7,8,9,10);
        Thread testD_6(functions::testD_6,11,12,13,14,15);
        testD_1.Run(); testD_1.Join();
        testD_2.Run(); testD_2.Join();
        testD_3.Run(); testD_3.Join();
        testD_4.Run(); testD_4.Join();
        testD_5.Run(); testD_5.Join();
        testD_6.Run(); testD_6.Join();
        ASSERT( functions::accumulator == (1+2+3+4+5+6+7+8+9+10+11+12+13+14+15) );

        functions testEf;
        Thread testE(testEf,1337);
        testE.Run();
        testE.Join();
        ASSERT( functions::accumulator == 1337 );

        functions testFf;
        testFf.accumulator2 = 0;
        ThreadMember testF(&functions::testF, &testFf, 100, 101 );
        testF.Run();
        testF.Join();
        ASSERT( testFf.accumulator2 == 201 );



        return true;
    }
    #endif
}
