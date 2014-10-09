#include "CommStream.hpp"
#include "Error/Error.hpp"
#include "Thread/Thread.hpp"
#include <climits>
#include <cstring>

#ifdef _WIN32
    //Include relevant Windows headers for networking
    #include <windows.h>
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #include <ctime>
#else
    //Include relevant Linux headers for networking
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <netdb.h>
    #include <unistd.h>
    #include <termios.h>
    #include <pthread.h>
    #include <errno.h>
    #include <signal.h>
    #include <fcntl.h>
    #include <sys/ioctl.h>
    #define closesocket(a) close(a)
    #define SOCKET unsigned int
    #define SOCKET_ERROR -1
    #define INVALID_SOCKET -1
#endif
#undef ERROR

namespace MUD{
    void CommStream::UseSocket(SOCKET sock){
        //Dump the provided sock into the current CommStream for use.
        myInternal = RefCounter<Internal>(new Internal(myInternal->MyReceiveType));
        myInternal->Connection = sock;
        myInternal->isconnected = true;
        Internal::PushStream(*this);
    }

    void CommStream::Terminate(){
        myInternal->Terminate();
    }

    void CommStream::PushData( char* data, size_t len ){
        return myInternal->PushData( data, len );
    }

    bool CommStream::CanSend(){
        return myInternal->CanSend();
    }

    CommStream::Internal* CommStream::Get(){
        return myInternal.get();
    }

    SOCKET CommStream::GetConnection(){
        return myInternal->Connection;
    }

    DLL_EXPORT CommStream::CommStream( CommStream::ReceiveType rectype ) : myInternal(new Internal(rectype)){

    }

    DLL_EXPORT CommStream::~CommStream(){
        Internal::PopStream(this);
    }

    DLL_EXPORT Error CommStream::Connect( std::string address, int port ){
        Error ret = myInternal->Connect( address, port );
        if( ret == Error::None )
            Internal::PushStream(*this);
        return ret;
    }

    DLL_EXPORT bool CommStream::Connected( ){
        return myInternal->Connected();
    }

    DLL_EXPORT Error CommStream::Disconnect( bool force ){
        return myInternal->Disconnect( force );
    }

    DLL_EXPORT Error CommStream::Listen( int port, std::function<void(CommStream)> func ){
        return myInternal->Listen( port, func );
    }

    DLL_EXPORT Error CommStream::ListenOn( std::string address, int port, std::function<void(CommStream)> func ){
        return myInternal->ListenOn( address, port, func );
    }

    DLL_EXPORT Error CommStream::Send( std::string message, bool important ){
        return myInternal->Send( message, important );
    }
    DLL_EXPORT Error CommStream::Send( void* message, size_t len, bool important ){
        return myInternal->Send( message, len, important );
    }

    DLL_EXPORT bool CommStream::HasData(){
        return myInternal->HasData();
    }

    DLL_EXPORT void CommStream::Flush(){
        Internal::ServiceSocket(myInternal.get());
    }

    DLL_EXPORT Error CommStream::SendFile( std::string path, bool important ){
        //Send a file on the current stream. No special treatment is given to the file data.
        //It is just dumped into the stream as-is.
        char buffer[1000];
        FILE* f = fopen( path.c_str(), "rb" );
        if( f == nullptr )
            return Error::FileNotFound;
        myInternal->transmitting = true;
        Error err = Error::None;
        int counter = 0;
        do{
            //This is to prevent many megabytes of data from accumulating in the send buffer
            if( myInternal->SendBufferSize() < 32 ){
                int amt = fread( buffer, 1, 1000, f );
                err = myInternal->Send(buffer, amt, false );
                counter = 0;
            }
            else{
                //If we have a saturated send buffer, then wait for 100 ms
                counter++;
                if( counter > 100 ) break;
                Thread::Sleep(100);
            }
            //If we encounter any error during the transfer, break the loop and report it.
            if( err != Error::None )
                break;
        } while( !feof(f) );
        fclose(f);
        myInternal->transmitting = false;
        return err;
    }

    DLL_EXPORT Error CommStream::Receive( std::string &message ){
        return myInternal->Receive( message );
    }

    DLL_EXPORT Error CommStream::Receive( char* message, size_t& len ){
        return myInternal->Receive( message, len );
    }

    DLL_EXPORT Error CommStream::Encrypt( CommStream::Encryption type ){
        return myInternal->Encrypt( type );
    }

    DLL_EXPORT Error CommStream::RegisterCallback( std::function<void()> f ){
        return myInternal->RegisterCallback( f );
    }

    DLL_EXPORT Error CommStream::ClearCallbacks( ){
        return myInternal->ClearCallbacks( );
    }

    DLL_EXPORT CommStream& CommStream::operator=(CommStream other){
        myInternal = other.myInternal;
        return *this;
    }

    DLL_EXPORT int CommStream::ServiceSockets(Internal *ptr){
        return Internal::ServiceSockets(ptr);
    }

    DLL_EXPORT int CommStream::SendBufferSize(){
        return myInternal->SendBufferSize();
    }


    #ifdef RunUnitTests
    //This function verifies that data is getting sent properly.
    static void Testcode( CommStream cs, void* data ){
        while( cs.Connected() ){
            std::string test;
            if( cs.Receive( test ) == Error::NotConnected ){
                *((int*)data) = 2;
                break;
            }

            Thread::Sleep(100);
            //Internal::ServiceSockets();
            if( test == "The test was successful" ){
                *((int*)data) = 1;
                break;
            }
        }
    }

    DLL_EXPORT bool CommStream::RunTests(){
        TEST("MUD::CommStream");


        //This test is highly specific. Optimally, a dedicated server would be set up just for testing this.
        //However, I don't have one running and my VPS has a 99.91% uptime IRC daemon that is known
        //to accept connections reliably.
        TEST("MUD::CommStream::Connect()");
        CommStream comm(CommStream::LINES);
        if( comm.Connect( "irc.frogbox.es", 6667 ) != Error::None ){
            FAIL("This could be a false positive - is irc.frogbox.es down?");
            return false;
        }

        TEST("MUD::CommStream::Receive()");
        TEST("MUD::CommStream::Send()");
        comm.Send( "NICK TestA\r\nUSER Test Test Test :Test\r\n");
        std::string buf;
        while( true ){
            Error a = comm.Receive(buf);
            if( a == Error::None ){

                //If the NICK is never sent, then the server will timeout
                if( buf.substr(0, 29) == "ERROR :Closing link: (unknown" ){
                    FAIL("Send() is not working as expected.");
                    return false;
                }
                //The server got the NICK request, so let's disconnect.
                if( buf.substr(0, 39) == ":irc.frogbox.es NOTICE Auth :Welcome to" ||
                    buf == ":irc.frogbox.es 462 TestA :You may not reregister" ){
                    comm.Send( "QUIT :BYE\r\n" );

                    comm.Disconnect( true );
                    break;
                }
                //If any other string is sent, then there's likely an issue.
                if( buf != ":irc.frogbox.es NOTICE Auth :*** Looking up your hostname..." &&
                    buf.substr(0, 52) != ":irc.frogbox.es NOTICE Auth :*** Found your hostname" &&
                    buf.substr(0, 39) != ":irc.frogbox.es NOTICE Auth :Welcome to" )
                    {
                        FAIL("This could be a false positive - is irc.frogbox.es down?");
                        return false;
                    }
            }
            if( a == Error::NotConnected )
                break;
            if( a == Error::NoData ){
            Thread::Sleep(1);
            //CommStream::ServiceSockets();

            }
        }
        TEST("MUD::CommStream::Listen()");
        int data = 0;
        int port = time( nullptr ) % 1000 + 1000;
        //Set up a server in its own thread and see if the callback is being called correctly
        Thread TestListen( std::bind( &CommStream::ListenOn, &comm, (std::string)"localhost", port,
                                (std::function<void(CommStream)>)(std::bind(Testcode, std::placeholders::_1, (void*)&data) ) ) );
        TestListen.Run();
        Thread::Sleep(100);
        CommStream comm2(CommStream::LINES);

        ASSERT( comm2.Connect( "localhost", port ) == Error::None );
        ASSERT( comm2.Send("The test was successful\n", true ) == Error::None );

        int iterations = 0;
        while( data == 0 && iterations++ < 10 ){
            Thread::Sleep(100);
        }
        comm2.Disconnect();
        comm.Disconnect();
        Thread::Sleep(1000);
        //Verify that comm.Disconnect stopped the server from listening.
        ASSERT( comm2.Connect( "localhost", port ) != Error::None )

        ASSERT(data == 1);

        return true;
    }

    #endif
}
