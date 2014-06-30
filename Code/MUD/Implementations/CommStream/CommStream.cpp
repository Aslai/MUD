#include "CommStream.hpp"
#include "Error/Error.hpp"
#include "Thread/Thread.hpp"
#include <climits>
#include <cstring>

#ifdef _WIN32
    #include <windows.h>
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #include <ctime>
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
    #include <fcntl.h>
    #include <sys/ioctl.h>
    #define closesocket(a) close(a)
    #define SOCKET unsigned int
    #define SOCKET_ERROR -1
    #define INVALID_SOCKET -1
#endif
#undef ERROR

namespace GlobalMUD{
        static void MakeNonblocking(SOCKET sock){

            #ifdef _WIN32
            static u_long iMode=1;
            ioctlsocket(sock,FIONBIO,&iMode);
            #else
            int flags = fcntl(sock, F_GETFL, 0);
            fcntl(sock, F_SETFL, flags | O_NONBLOCK);
            #endif
        }
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

        Error CommStream::SendFile( std::string path, bool important ){
            char buffer[1000];
            FILE* f = fopen( path.c_str(), "rb" );
            if( f == NULL )
                return Error::FileNotFound;
            Internal->transmitting = true;
            Error err = Error::None;
            int counter = 0;
            do{
                if( Internal->SendBufferSize() < 32 ){
                    int amt = fread( buffer, 1, 1000, f );
                    err = Internal->Send(buffer, amt, false );
                    counter = 0;
                }
                else{
                    counter++;
                    if( counter > 100 ) break;
                    Thread::Sleep(100);
                }
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


                    if( buf.substr(0, 29) == "ERROR :Closing link: (unknown" ){
                        FAIL("Send() is not working as expected.");
                        return false;
                    }
                    if( buf.substr(0, 39) == ":irc.frogbox.es NOTICE Auth :Welcome to" ||
                        buf == ":irc.frogbox.es 462 TestA :You may not reregister" ){
                        comm.Send( "QUIT :BYE\r\n" );

                        comm.Disconnect( true );
                        break;
                    }
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
            Thread TestListen( std::bind( &CommStream::ListenOn, &comm, (std::string)"localhost", port,
                                    (std::function<void(CommStream)>)(std::bind(Testcode, std::placeholders::_1, (void*)&data) ) ) );
            TestListen.Run();
            //TestListen.Join();
            //comm.Listen(80, Testcode, 0);
            Thread::Sleep(100);
            CommStream comm2(CommStream::LINES);

            ASSERT( comm2.Connect( "localhost", port ) == Error::None );
            ASSERT( comm2.Send("The test was successful\n", true ) == Error::None );
            //CommStream::ServiceSockets();

            int iterations = 0;
            while( data == 0 && iterations++ < 10 ){
                Thread::Sleep(100);
                //CommStream::ServiceSockets();
            }
            comm.Disconnect();
            Thread::Sleep(100);
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
    CommStreamInternal::Message::Message(const char* message, size_t len, int flagss ) : msg((char*)malloc(len+1), free) {
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
        connecting = 0;
        disconnecting = false;
        transmitting = false;
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
        if( host == 0 ) return Error::InvalidHost;

        //Connect the socket
        SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

        if( sock == (unsigned)SOCKET_ERROR ) return Error::SocketFailure;
        sockaddr_in clientService;
        clientService.sin_family = AF_INET;
        clientService.sin_port = htons( port );

        int result = SOCKET_ERROR;
        for( int i = 0; host->h_addr_list[i] != NULL && i < 10; ++i ){
            clientService.sin_addr.s_addr = *((unsigned long long*) host->h_addr_list[i]);
            result = connect( sock, (sockaddr*) &clientService, sizeof(clientService));
            if( result == 0 ) break;
        }
        if( result == SOCKET_ERROR ) return Error::ConnectionFailure;
        MakeNonblocking(sock);
        Connection = sock;
        isconnected = true;
        connecting = 0;
        disconnecting = false;
        return Error::None;
    }

    bool CommStreamInternal::Connected( ){
        if( !isconnected )
            return false;
        if( Connection == (unsigned)SOCKET_ERROR )
            return false;
        //sockaddr dummysockaddr;
        //unsigned int dummyint = sizeof( sockaddr );

        //if( getsockname( Connection, &dummysockaddr, &dummyint ) != SOCKET_ERROR ){
        //    return true;
        //}
        if( isconnected ){
            return true;
        }
        else{
            isconnected = false;
            return false;
        }
    }

    Error CommStreamInternal::Disconnect( bool force ){
        if( !force && ImportantMessages > 0 )
            return Error::ImportantOperation;
        disconnecting = force?2:1;
        return Error::None;
    }

    Error CommStreamInternal::Listen( int port, std::function<void(CommStream)> func ){
        return ListenOn( "any", port, func);
    }

    Error CommStreamInternal::ListenOn( std::string address, int port, std::function<void(CommStream)> func ){
        //Look up the host / resolve IP
        unsigned long long myhost;
        if( address == "any" ){
            myhost = INADDR_ANY;
        }
        else {
            hostent* host = gethostbyname( address.c_str() );
            if( host == 0 ) return Error::InvalidHost;
            myhost = *((unsigned long long*) host->h_addr_list[0]);
        }

        //Establish the listening socket
        SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (sock == (unsigned)SOCKET_ERROR) {return Error::BindFailure;}
        sockaddr_in service;

        service.sin_family = AF_INET;
        service.sin_addr.s_addr = myhost;
        service.sin_port = htons( port );
        if (bind(sock,(sockaddr*) (&service), sizeof(service)) < 0) {
            return Error::BindFailure;
        }
        listen( sock, 10 );
        if( sock == (unsigned)SOCKET_ERROR ) return Error::ListenFailure;

        SOCKET asock;
        aborted = false;
        while(!aborted){
            struct sockaddr addr;
            socklen_t addrlen = sizeof( addr );
            asock = accept( sock, &addr, &addrlen );
            if( asock == (unsigned)INVALID_SOCKET ){
                break;
            }
            MakeNonblocking(asock);

            CommStream toPass(MyReceiveType);
            toPass.UseSocket(asock);
            func( toPass );
            #ifdef DISABLETHREADS
           // break;
            #endif // DISABLETHREADS
        }
        return Error::ConnectionFailure;
    }

    Error CommStreamInternal::Send( std::string message, bool important ){
        if( !isconnected )
            return Error::NotConnected;
        if( important )
            ImportantMessages ++;
        MyLock.Lock();
        SendBuffer.push_back( Message(message, important?CommStreamInternal::Message::IMPORTANT:CommStreamInternal::Message::NONE) );
        MyLock.Unlock();
        CommStream::ServiceSockets(this);
        return Error::None;
    }

    Error CommStreamInternal::Send( void* message, size_t len, bool important ){
        if( !isconnected )
            return Error::NotConnected;
        if( important )
            ImportantMessages ++;
        MyLock.Lock();
        SendBuffer.push_back( Message((const char*)message, len, important?CommStreamInternal::Message::IMPORTANT:CommStreamInternal::Message::NONE) );
        MyLock.Unlock();
        CommStream::ServiceSockets(this);
        return Error::None;
    }

    Error CommStreamInternal::Receive( std::string &message ){
        if( !isconnected )
            return Error::NotConnected;
        CommStream::ServiceSockets(this);
        if( RecvBuffer.size() == 0 )
            return Error::NoData;
        MyLock.Lock();
        message = (const char*) RecvBuffer.front().msg.get();
        RecvBuffer.pop_front();
        MyLock.Unlock();
        return Error::None;
    }

    Error CommStreamInternal::Receive( char* message, size_t& len ){
        size_t len2 = len;
        len = 0;
        if( !isconnected )
            return Error::NotConnected;
        CommStream::ServiceSockets(this);
        if( RecvBuffer.size() == 0 )
            return Error::NoData;
        MyLock.Lock();
        while( len2 > 0 && RecvBuffer.size() > 0 ){
            //std::shared_ptr<char> temp;
            const char* temp = RecvBuffer.front().msg.get();
            size_t length = RecvBuffer.front().length;
            int flags = RecvBuffer.front().flags;
            size_t copylength = length < len2 ? length : len2;

            memcpy( message, temp, copylength );
            length -= copylength;
            len2 -= copylength;
            message += copylength;
            len += copylength;
            if( length > 0 ){
                Message m( temp + copylength, length, flags );

                RecvBuffer.pop_front();
                RecvBuffer.push_front(m);
            }
            else
                RecvBuffer.pop_front();
        }
        MyLock.Unlock();
        return Error::None;
    }

    Error CommStreamInternal::Encrypt( CommStream::Encryption type ){
        return Error::InvalidScheme;
    }

    Error CommStreamInternal::RegisterCallback( std::function<void()> f ){
        callbacks.push_back( f );
        return Error::None;
    }

    int CommStreamInternal::ServiceSocket( CommStreamInternal *ptr ){
            char recvbuf[NETBUFFERSIZE];
            Lock.Lock();
            int ret = 0;

            if( !ptr->Connected() ){
                ptr->Terminate();
                //CommStreams.erase( CommStreams.begin() + i );
                return ret;
            }

            bool doRepeat;
            bool die = false;;
            do {
                doRepeat = false;
                int result = recv( ptr->Connection, recvbuf, NETBUFFERSIZE, 0 );
                if( result > 0 )
                    ret++;

                if( result == 0 ){
                    ptr->Terminate();
                    die = true;
                    break;
                }
                else if( result == SOCKET_ERROR ){

                    #ifdef _WIN32
                    switch(WSAGetLastError()){
                    case WSAEMSGSIZE: doRepeat = true;
                    case WSAEWOULDBLOCK:
                    case WSAEINPROGRESS:
                    case WSAEINTR: break;
                    default:

                        ptr->Terminate();
                        die = true;
                        break;
                    }
                    #else
                    switch(errno){
                    case EMSGSIZE: doRepeat = true;
                    //case EAGAIN:
                    case EWOULDBLOCK: break;
                    default:
                        ptr->Terminate();
                        die = true;
                        break;
                    }
                    #endif

                }
                else {
                    ptr->MyLock.Lock();
                    ptr->PushData( recvbuf, result );
                    ptr->MyLock.Unlock();
                    auto iter = ptr->callbacks.begin();
                    auto e = ptr->callbacks.end();
                    while( iter != e ){
                        (*iter)();
                        iter++;
                    }
                }
            } while( doRepeat );

            if( !die ){
                while( ptr->CanSend() ){
                    ptr->MyLock.Lock();
                    Message msg = ptr->GetSend();
                    ptr->MyLock.Unlock();
                    int size = msg.length;
                    while( size > 0 ){

                        int result = send( ptr->Connection, msg.msg.get() + msg.length - size, size, 0 );
                        if( result > 0 )
                            ret++;
                        if( result == 0 ){
                            ptr->Terminate();
                            die = true;
                            break;
                        }
                        else if( result == SOCKET_ERROR ){
                            #ifdef _WIN32
                            switch(WSAGetLastError()){
                            case WSAENOBUFS: doRepeat = true;
                            case WSAEWOULDBLOCK:
                            case WSAEINPROGRESS:
                            case WSAEINTR: break;
                            default: ptr->Terminate();
                                die = true;
                                size = 0;
                                break;
                            }
                        #else
                        switch(errno){
                        case EMSGSIZE: doRepeat = true;
                        //case EAGAIN:
                        case EWOULDBLOCK: break;
                        default:ptr->Terminate();
                                die = true;
                                size = 0;
                                break;
                        }
                        #endif
                        }
                        else {
                            size -= result;
                        }
                    }
                    if( die )
                        break;
                }
            }
            if( ptr->disconnecting == 2 || (!die && ptr->disconnecting == 1 && !ptr->CanSend() && !ptr->transmitting) ){
                closesocket( ptr->Connection );
                ptr->isconnected = false;
                ptr->aborted = true;
            }
            if( die ){
                //CommStreams.erase( CommStreams.begin() + i-- );
            }
            Lock.Unlock();
            return ret;
    }

    int CommStreamInternal::ServiceSockets( CommStreamInternal *ptr ){
        Lock.Lock();
        int ret = 0;
        if( ptr != NULL ){
            ret = ServiceSocket( ptr );
        }
        else {
            for( unsigned int i = 0; i < CommStreams.size() || i > INT_MAX - 2; ++i ){
                ret += ServiceSocket( CommStreams[i].Get() );
            }
        }
        Lock.Unlock();
        return ret;
    }

    int CommStreamInternal::SendBufferSize(){
        return SendBuffer.size();
    }

    void CommStreamInternal::PushStream( CommStream topush ){
        Lock.Lock();
        CommStreams.push_back(topush);
        Lock.Unlock();
    }
    Mutex CommStreamInternal::Lock;
    std::vector<CommStream> CommStreamInternal::CommStreams;
}
