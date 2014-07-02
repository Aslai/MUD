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

namespace GlobalMUD{
    void CommStream::UseSocket(SOCKET sock){
        //Dump the provided sock into the current CommStream for use.
        Internal = RefCounter<CommStreamInternal>(new CommStreamInternal(Internal->MyReceiveType));
        Internal->Connection = sock;
        Internal->isconnected = true;
        CommStreamInternal::PushStream(*this);
    }

    void CommStream::Terminate(){
        Internal->Terminate();
    }

    void CommStream::PushData( char* data, size_t len ){
        return Internal->PushData( data, len );
    }

    bool CommStream::CanSend(){
        return Internal->CanSend();
    }

    CommStreamInternal* CommStream::Get(){
        return Internal.get();
    }

    SOCKET CommStream::GetConnection(){
        return Internal->Connection;
    }

    CommStream::CommStream( CommStream::ReceiveType rectype ) : Internal(new CommStreamInternal(rectype)){

    }

    CommStream::~CommStream(){
        CommStreamInternal::PopStream(this);
    }

    Error CommStream::Connect( std::string address, int port ){
        Error ret = Internal->Connect( address, port );
        if( ret == Error::None )
            CommStreamInternal::PushStream(*this);
        return ret;
    }

    bool CommStream::Connected( ){
        return Internal->Connected();
    }

    Error CommStream::Disconnect( bool force ){
        return Internal->Disconnect( force );
    }

    Error CommStream::Listen( int port, std::function<void(CommStream)> func ){
        return Internal->Listen( port, func );
    }

    Error CommStream::ListenOn( std::string address, int port, std::function<void(CommStream)> func ){
        return Internal->ListenOn( address, port, func );
    }

    Error CommStream::Send( std::string message, bool important ){
        return Internal->Send( message, important );
    }
    Error CommStream::Send( void* message, size_t len, bool important ){
        return Internal->Send( message, len, important );
    }

    void CommStream::Flush(){
        CommStreamInternal::ServiceSocket(Internal.get());
    }

    Error CommStream::SendFile( std::string path, bool important ){
        //Send a file on the current stream. No special treatment is given to the file data.
        //It is just dumped into the stream as-is.
        char buffer[1000];
        FILE* f = fopen( path.c_str(), "rb" );
        if( f == NULL )
            return Error::FileNotFound;
        Internal->transmitting = true;
        Error err = Error::None;
        int counter = 0;
        do{
            //This is to prevent many megabytes of data from accumulating in the send buffer
            if( Internal->SendBufferSize() < 32 ){
                int amt = fread( buffer, 1, 1000, f );
                err = Internal->Send(buffer, amt, false );
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
        Internal->transmitting = false;
        return err;
    }

    Error CommStream::Receive( std::string &message ){
        return Internal->Receive( message );
    }

    Error CommStream::Receive( char* message, size_t& len ){
        return Internal->Receive( message, len );
    }

    Error CommStream::Encrypt( CommStream::Encryption type ){
        return Internal->Encrypt( type );
    }

    Error CommStream::RegisterCallback( std::function<void()> f ){
        return Internal->RegisterCallback( f );
    }

    Error CommStream::ClearCallbacks( ){
        return Internal->ClearCallbacks( );
    }

    CommStream& CommStream::operator=(CommStream other){
        Internal = other.Internal;
        return *this;
    }

    int CommStream::ServiceSockets(CommStreamInternal *ptr){
        return CommStreamInternal::ServiceSockets(ptr);
    }

    int CommStream::SendBufferSize(){
        return Internal->SendBufferSize();
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
            //CommStreamInternal::ServiceSockets();
            if( test == "The test was successful" ){
                *((int*)data) = 1;
                break;
            }
        }
    }

    bool CommStream::RunTests(){
        TEST("GlobalMUD::CommStream");


        //This test is highly specific. Optimally, a dedicated server would be set up just for testing this.
        //However, I don't have one running and my VPS has a 99.91% uptime IRC daemon that is known
        //to accept connections reliably.
        TEST("GlobalMUD::CommStream::Connect()");
        CommStream comm(CommStream::LINES);
        if( comm.Connect( "irc.frogbox.es", 6667 ) != Error::None ){
            FAIL("This could be a false positive - is irc.frogbox.es down?");
            return false;
        }

        TEST("GlobalMUD::CommStream::Receive()");
        TEST("GlobalMUD::CommStream::Send()");
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
        TEST("GlobalMUD::CommStream::Listen()");
        int data = 0;
        int port = time(0) % 1000 + 1000;
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
