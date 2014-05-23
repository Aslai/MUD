#include "CommStream.hpp"
#include "Global/Error.hpp"
#include "Global/Thread.hpp"

#ifdef _WIN32
    #include <windows.h>
    #include <winsock2.h>
    #include <ws2tcpip.h>
#else
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <netdb.h>
    #include <unistd.h>
    #include <termios.h>
    #include <pthread.h>
    #include <errno.h>
    #include <signal.h>
    #define SOCKET unsigned int
#endif
#undef ERROR

namespace GlobalMUD{

        void CommStream::UseSocket(SOCKET sock){
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

        }

        Error CommStream::Connect( std::string address, int port ){
            Error ret = Internal->Connect( address, port );
            if( ret == ERROR::None )
                CommStreamInternal::PushStream(*this);
            return ret;
        }

        bool CommStream::Connected( ){
            return Internal->Connected();
        }

        Error CommStream::Disconnect( bool force ){
            return Internal->Disconnect( force );
        }

        Error CommStream::Listen( int port, void(*func)(CommStream stream, void* data), void* data ){
            return Internal->Listen( port, func, data );
        }

        Error CommStream::ListenOn( std::string address, int port, void(*func)(CommStream stream, void* data), void* data ){
            return Internal->ListenOn( address, port, func, data );
        }

        Error CommStream::Send( std::string message, bool important ){
            return Internal->Send( message, important );
        }

        Error CommStream::Receive( std::string &message ){
            return Internal->Receive( message );
        }

        Error CommStream::Encrypt( CommStream::Encryption type ){
            return Internal->Encrypt( type );
        }

        CommStream& CommStream::operator=(CommStream other){
            Internal = other.Internal;
            return *this;
        }

        void CommStream::ServiceSockets(){
            return CommStreamInternal::ServiceSockets();
        }


        #ifdef RunUnitTests
        static void Testcode( CommStream cs, void* data ){
            while( cs.Connected() ){
                std::string test;
                if( cs.Receive( test ) == ERROR::NotConnected ){
                    *((int*)data) = 2;
                    break;
                }

                Sleep(100);
                CommStreamInternal::ServiceSockets();
                if( test == "The test was successful" ){
                    *((int*)data) = 1;
                    break;
                }
            }
        }

        bool CommStream::RunTests(){
            TEST("GlobalMUD::CommStream");
            //FAIL("GlobalMUD::CommStream not yet implemented.")
            TEST("GlobalMUD::CommStream::Connect()");
            CommStream comm(CommStream::LINES);
            if( comm.Connect( "irc.frogbox.es", 6667 ) != ERROR::None ){
                FAIL("This could be a false positive - is irc.frogbox.es down?");
                return false;
            }
            CommStream::ServiceSockets();
            TEST("GlobalMUD::CommStream::Receive()");
            TEST("GlobalMUD::CommStream::Send()");
            comm.Send( "NICK TestA\r\nUSER Test Test Test :Test\r\n");
            std::string buf;
            while( true ){
                Error a = comm.Receive(buf);
                if( a == ERROR::None ){
                    //printf("%s\n", buf.c_str());


                    if( buf.substr(0, 29) == "ERROR :Closing link: (unknown" ){
                        FAIL("Send() is not working as expected.");
                        return false;
                    }
                    if( buf.substr(0, 39) == ":irc.frogbox.es NOTICE Auth :Welcome to" ||
                        buf == ":irc.frogbox.es 462 TestA :You may not reregister" ){
                        comm.Send( "QUIT :BYE\r\n" );
                        CommStream::ServiceSockets();
                        comm.Disconnect( true );
                        break;
                    }
                    if( buf != ":irc.frogbox.es NOTICE Auth :*** Looking up your hostname..." &&
                        buf != ":irc.frogbox.es NOTICE Auth :*** Found your hostname (c-67-160-91-142.hsd1.wa.comcast.net) -- cached" &&
                        buf.substr(0, 39) != ":irc.frogbox.es NOTICE Auth :Welcome to" )
                        {
                            FAIL("This could be a false positive - is irc.frogbox.es down?");
                            return false;
                        }
                }
                if( a == ERROR::NotConnected )
                    break;
                if( a == ERROR::NoData ){
                    Sleep(1000);
                    CommStream::ServiceSockets();
                }
            }
            TEST("GlobalMUD::CommStream::Listen()");
            int data = 0;
            ThreadMember TestListen( &CommStream::Listen, &comm, 5555, Testcode, (void*)&data );
            TestListen.Run();
            //TestListen.Join();
            //comm.Listen(80, Testcode, 0);
            Thread::Sleep(100);
            CommStream comm2(CommStream::LINES);

            ASSERT( comm2.Connect( "localhost", 5555 ) == ERROR::None );
            ASSERT( comm2.Send("The test was successful\n", true ) == ERROR::None );
            CommStream::ServiceSockets();
            int iterations = 0;
            while( data == 0 && iterations++ < 10 ){
                Thread::Sleep(100);
                CommStream::ServiceSockets();
            }
            comm.Disconnect();
            Sleep(100);
            TestListen.Kill();
            ASSERT(data == 1);

            return true;
        }

        #endif
    CommStreamInternal::Message::Message(std::string message, int flagss ) : msg((char*)malloc(message.length()+1), free) {
        memcpy( msg.get(), message.c_str(), message.length() );
        msg.get()[message.length()] = 0;
        length = message.length();
        flags = flagss;
    }
    CommStreamInternal::Message::Message(const char* message, size_t len, int flagss ) : msg((char*)malloc(len), free) {
        memcpy( msg.get(), message, len );
        msg.get()[len] = 0;
        length = len;
        flags = flagss;
    }

    CommStreamInternal::CommStreamInternal( CommStream::ReceiveType rectype ){

        MyReceiveType = rectype;
        MyEncryption = CommStream::NONE;
        MyCipher = new Ciphers::Cipher();
        isconnected = false;
        ImportantMessages = 0;
        RecvLinesBuffer = "";
    }

    CommStreamInternal::~CommStreamInternal(){
        if( MyCipher )
            delete MyCipher;
    }

    void CommStreamInternal::Terminate(){
        Disconnect( true );
    }

    void CommStreamInternal::PushData( char* data, size_t len ){
        if( MyReceiveType == CommStream::BINARY ){
            Message msg(data, len, CommStream::NONE);
            RecvBuffer.push_back( msg );
        }
        else{
            while(len--){
                RecvLinesBuffer += *(data++);

                if( RecvLinesBuffer.back() == '\n' ){
                    while( RecvLinesBuffer.back() == '\r' || RecvLinesBuffer.back() == '\n' ){
                        RecvLinesBuffer.pop_back();
                    }
                    Message msg(RecvLinesBuffer, CommStream::NONE);
                    RecvBuffer.push_back( msg );
                    RecvLinesBuffer = "";
                }
            }
        }
    }

    bool CommStreamInternal::CanSend(){
        return SendBuffer.size() > 0;
    }

    CommStreamInternal::Message CommStreamInternal::GetSend(){
        Message toReturn = SendBuffer.front();
        SendBuffer.pop_front();
        return toReturn;
    }

    Error CommStreamInternal::Connect( std::string address, int port ){
        //Look up the host / resolve IP
        hostent* host = gethostbyname( address.c_str() );
        if( host == 0 ) return ERROR::InvalidHost;

        //Connect the socket
        SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

        if( sock == (unsigned)SOCKET_ERROR ) return ERROR::SocketFailure;
        sockaddr_in clientService;
        clientService.sin_family = AF_INET;
        clientService.sin_port = htons( port );

        int result;
        for( int i = 0; host->h_addr_list[i] != NULL && i < 10; ++i ){
            clientService.sin_addr.s_addr = *((unsigned long long*) host->h_addr_list[i]);
            result = connect( sock, (sockaddr*) &clientService, sizeof(clientService));
            if( result == 0 ) break;
        }
        if( result == SOCKET_ERROR ) return ERROR::ConnectionFailure;
        static u_long iMode=1;
        ioctlsocket(sock,FIONBIO,&iMode);
        Connection = sock;
        isconnected = true;
        return ERROR::None;
    }

    bool CommStreamInternal::Connected( ){
        if( !isconnected )
            return false;
        if( Connection == (unsigned)SOCKET_ERROR )
            return false;
        sockaddr dummysockaddr;
        int dummyint = sizeof( sockaddr );
        if( getsockname( Connection, &dummysockaddr, &dummyint ) != SOCKET_ERROR ){
            return true;
        }
        else{
            isconnected = false;
            return false;
        }
    }

    Error CommStreamInternal::Disconnect( bool force ){
        if( !force && ImportantMessages > 0 )
            return ERROR::ImportantOperation;
        if( closesocket( Connection ) == SOCKET_ERROR ){
            return ERROR::NotConnected;
        }
        isconnected = false;
        aborted = true;
        Lock.Lock();

        Lock.Unlock();
        return ERROR::None;
    }

    Error CommStreamInternal::Listen( int port, void(*func)(CommStream stream, void* data), void* data ){
        return ListenOn( "any", port, func, data );
    }

    Error CommStreamInternal::ListenOn( std::string address, int port, void(*func)(CommStream stream, void* data), void* data ){
        //Look up the host / resolve IP
        unsigned long long myhost;
        if( address == "any" ){
            myhost = INADDR_ANY;
        }
        else {
            hostent* host = gethostbyname( address.c_str() );
            if( host == 0 ) return ERROR::InvalidHost;
            myhost = *((unsigned long long*) host->h_addr_list[0]);
        }

        //Establish the listening socket
        SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (sock == (unsigned)SOCKET_ERROR) {return ERROR::BindFailure;}
        sockaddr_in service;

        service.sin_family = AF_INET;
        service.sin_addr.s_addr = myhost;
        service.sin_port = htons( port );
        if (bind(sock,(sockaddr*) (&service), sizeof(service)) < 0) {
            return ERROR::BindFailure;
        }
        listen( sock, 10 );
        if( sock == (unsigned)SOCKET_ERROR ) return ERROR::ListenFailure;

        SOCKET asock;
        aborted = false;
        while(!aborted){
            asock = accept( sock, 0, 0 );
            if( asock == (unsigned)INVALID_SOCKET ){
                break;
            }
            static u_long iMode=1;
            ioctlsocket(asock,FIONBIO,&iMode);

            CommStream toPass(MyReceiveType);
            toPass.UseSocket(asock);
            func( toPass, data );
        }
        return ERROR::ConnectionFailure;
    }

    Error CommStreamInternal::Send( std::string message, bool important ){
        if( !isconnected )
            return ERROR::NotConnected;
        if( important )
            ImportantMessages ++;
        SendBuffer.push_back( Message(message, important?CommStreamInternal::Message::IMPORTANT:CommStreamInternal::Message::NONE) );
        return ERROR::None;
    }

    Error CommStreamInternal::Receive( std::string &message ){
        if( !isconnected )
            return ERROR::NotConnected;
        if( RecvBuffer.size() == 0 )
            return ERROR::NoData;
        message = (const char*) RecvBuffer.front().msg.get();
        RecvBuffer.pop_front();
        return ERROR::None;
    }

    Error CommStreamInternal::Encrypt( CommStream::Encryption type ){
        return ERROR::InvalidScheme;
    }


    void CommStreamInternal::ServiceSockets(){
        char recvbuf[NETBUFFERSIZE];
        Lock.Lock();

        for( unsigned int i = 0; i < CommStreams.size(); ++i ){
            if( !CommStreams[i].Connected() ){
                CommStreams[i].Terminate();
                CommStreams.erase( CommStreams.begin() + i-- );
                continue;
            }
            bool doRepeat;
            bool die = false;;
            do {
                doRepeat = false;
                int result = recv( CommStreams[i].GetConnection(), recvbuf, NETBUFFERSIZE, 0 );
                //printf( "%d\t%s\n\n", result, recvbuf );
                if( result == 0 ){
                    CommStreams[i].Terminate();
                    CommStreams.erase( CommStreams.begin() + i-- );
                    die = true;
                    break;
                }
                else if( result == SOCKET_ERROR ){
                        //printf("\n%d\n", WSAGetLastError() );
                    switch(WSAGetLastError()){
                    case WSAEMSGSIZE: doRepeat = true;
                    case WSAEWOULDBLOCK:
                    case WSAEINPROGRESS:
                    case WSAEINTR: break;
                    default:

                        CommStreams[i].Terminate();
                        CommStreams.erase( CommStreams.begin() + i-- );
                        die = true;
                        break;
                    }
                }
                else {
                    CommStreams[i].PushData( recvbuf, result );
                }
            } while( doRepeat );

            if( die )
                continue;

            while( CommStreams[i].CanSend() ){
                Message msg = CommStreams[i].Get()->GetSend();
                int size = msg.length;
                while( size > 0 ){
                    int result = send( CommStreams[i].GetConnection(), msg.msg.get() + msg.length - size, size, 0 );
                    if( result == 0 ){
                        CommStreams[i].Terminate();
                        CommStreams.erase( CommStreams.begin() + i-- );
                        die = true;
                        break;
                    }
                    else if( result == SOCKET_ERROR ){
                        switch(WSAGetLastError()){
                        case WSAENOBUFS: doRepeat = true;
                        case WSAEWOULDBLOCK:
                        case WSAEINPROGRESS:
                        case WSAEINTR: break;
                        default: CommStreams[i].Terminate();
                            CommStreams.erase( CommStreams.begin() + i-- );
                            die = true;
                            size = 0;
                            break;
                        }
                    }
                    else {
                        size -= result;
                    }
                }
                if( die )
                    break;
            }
        }
        Lock.Unlock();
    }

    void CommStreamInternal::PushStream( CommStream topush ){
        Lock.Lock();
        CommStreams.push_back(topush);
        Lock.Unlock();
    }
    Mutex CommStreamInternal::Lock;
    std::vector<CommStream> CommStreamInternal::CommStreams;
}
