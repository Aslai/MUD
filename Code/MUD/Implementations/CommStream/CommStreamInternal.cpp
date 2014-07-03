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
    #define INVALID_SOCKET (unsigned int)-1
#endif
#undef ERROR

namespace GlobalMUD{
    static void MakeNonblocking(SOCKET sock){
        //Use ioctlsocket/fcntl to make sock nonblocking
        #ifdef _WIN32
        static u_long iMode=1;
        ioctlsocket(sock,FIONBIO,&iMode);
        #else
        int flags = fcntl(sock, F_GETFL, 0);
        fcntl(sock, F_SETFL, flags | O_NONBLOCK);
        #endif
    }

    //Must initialize the shared_ptr in the initialization
    CommStream::Internal::Message::Message(std::string message, int flagss ) : msg((char*)malloc(message.length()+1), free) {
        //Fill the shared_ptr with information from message
        memcpy( msg.get(), message.c_str(), message.length() );
        msg.get()[message.length()] = 0;
        length = message.length();
        flags = flagss;
    }
    CommStream::Internal::Message::Message(const char* message, size_t len, int flagss ) : msg((char*)malloc(len+1), free) {
        //Fill the shared_ptr with information from message
        memcpy( msg.get(), message, len );
        msg.get()[len] = 0;
        length = len;
        flags = flagss;
    }

    CommStream::Internal::Internal( CommStream::ReceiveType rectype ){
        MyReceiveType = rectype;
        MyEncryption = CommStream::NONE;
        MyCipher = new Ciphers::Cipher();
        isconnected = false;
        ImportantMessages = 0;
        RecvLinesBuffer = "";
        connecting = 0;
        disconnecting = false;
        transmitting = false;
        isServer = false;
        Connection = INVALID_SOCKET;
        ID = -1;
    }

    CommStream::Internal::~Internal(){
        if( MyCipher )
            delete MyCipher;
        MyLock.Unlock();
        if( Connection != INVALID_SOCKET )
            closesocket( Connection );
    }

    void CommStream::Internal::Terminate(){

        Disconnect( true );
    }

    //Push data onto the data buffer for later retrieval with Receive()
    void CommStream::Internal::PushData( char* data, size_t len ){

        if( MyReceiveType == CommStream::BINARY ){
            //If the data expected is binary, just push it onto the end as-is
            Message msg(data, len, CommStream::NONE);
            RecvBuffer.push_back( msg );
        }
        else{
            //If the data expected is to be broken up into lines, break it into chunks at every newline
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

    bool CommStream::Internal::CanSend(){
        return SendBuffer.size() > 0;
    }

    //Get the next bit of data that needs to be sent
    CommStream::Internal::Message CommStream::Internal::GetSend(){
        Message toReturn = SendBuffer.front();
        SendBuffer.pop_front();
        return toReturn;
    }

    Error CommStream::Internal::Connect( std::string address, int port ){
        //Look up the host / resolve IP
        hostent* host = gethostbyname( address.c_str() );
        if( host == nullptr )
        {
            //If we're on Windows, make sure that the failure isn't due to
            //Winsock not being initialized.
            #ifdef _WIN32
            if( WSAGetLastError() == WSANOTINITIALISED ){

                WSADATA globalWSAData;
                WSAStartup( MAKEWORD(2, 2), &globalWSAData );
                host = gethostbyname( address.c_str() );
                if( host == nullptr )
                    return Error::InvalidHost;
            }
            else
            #endif
            return Error::InvalidHost;
        }

        //Connect the socket
        SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

        if( sock == (unsigned)SOCKET_ERROR ) return Error::SocketFailure;
        sockaddr_in clientService;
        clientService.sin_family = AF_INET;
        clientService.sin_port = htons( port );

        int result = SOCKET_ERROR;
        for( int i = 0; host->h_addr_list[i] != nullptr && i < 10; ++i ){
            clientService.sin_addr.s_addr = *((unsigned long long*) host->h_addr_list[i]);
            result = connect( sock, (sockaddr*) &clientService, sizeof(clientService));
            if( result == 0 ) break;
        }
        if( result == SOCKET_ERROR ) return Error::ConnectionFailure;
        //Put the connection into a valid state
        MakeNonblocking(sock);
        Connection = sock;
        isconnected = true;
        connecting = 0;
        disconnecting = false;
        return Error::None;
    }

    bool CommStream::Internal::Connected( ){
        //Evaluate this socket for validity and fill any buffers with data waiting on the network stack.
        ServiceSockets( this );
        //if( !isconnected || Connection == (unsigned)SOCKET_ERROR )
        if( Connection == (unsigned)SOCKET_ERROR )
            return false;
        return isconnected;
    }

    Error CommStream::Internal::Disconnect( bool force ){
        if( !force && ImportantMessages > 0 )
            return Error::ImportantOperation;
        //Signal to the connection handler that we have a desire to disconnect.
        //2 means that it's being forced. This is to allow the connection to finish
        //flushing data before the connection is aborted.
        disconnecting = force?2:1;

        //If this object is acting as a daemon, then kill the listening socket.
        if( Connection != INVALID_SOCKET && isServer ){
            closesocket( Connection );
            Connection = INVALID_SOCKET;
        }
        return Error::None;
    }
    //For listening on any address
    Error CommStream::Internal::Listen( int port, std::function<void(CommStream)> func ){
        return ListenOn( "any", port, func);
    }

    Error CommStream::Internal::ListenOn( std::string address, int port, std::function<void(CommStream)> func ){
        //Establish the listening socket
        SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (sock == (unsigned)SOCKET_ERROR) {
            //If we're on Windows, make sure that the failure isn't due to
            //Winsock not being initialized.
            #ifdef _WIN32
            if( WSAGetLastError() == WSANOTINITIALISED ){

                WSADATA globalWSAData;
                WSAStartup( MAKEWORD(2, 2), &globalWSAData );
                sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
                if (sock == (unsigned)SOCKET_ERROR)
                    return Error::SocketFailure;
            }
            else
            #endif
            return Error::SocketFailure;
        }

        disconnecting = 0;

        //Look up the host / resolve IP
        unsigned long long myhost;
        if( address == "any" ){
            myhost = INADDR_ANY;
        }
        else {
            //Winsock is definitely initialized by this point
            hostent* host = gethostbyname( address.c_str() );
            if( host == nullptr ) return Error::InvalidHost;
            myhost = *((unsigned long long*) host->h_addr_list[0]);
        }

        //Put the listening socket into a valid state
        sockaddr_in service;
        service.sin_family = AF_INET;
        service.sin_addr.s_addr = myhost;
        service.sin_port = htons( port );

        if (bind(sock,(sockaddr*) (&service), sizeof(service)) != 0) {
            printf("Err: %d\n", errno );
            return Error::BindFailure;
        }
        listen( sock, 10 );

        //Save the listening connection for use by Disconnect() to abort a pending accept()
        Connection = sock;
        isServer = true;

        if( sock == (unsigned)SOCKET_ERROR ) return Error::ListenFailure;

        SOCKET asock;
        aborted = false;
        while(!aborted){
            struct sockaddr addr;
            socklen_t addrlen = sizeof( addr );
            asock = accept( sock, &addr, &addrlen );
            if( asock == (unsigned)INVALID_SOCKET || disconnecting != 0 ){
                break;
            }
            MakeNonblocking(asock);

            CommStream toPass(MyReceiveType);
            toPass.UseSocket(asock);

            //Run the callback
            func( toPass );
        }
        isServer = false;
        return Error::ConnectionFailure;
    }

    Error CommStream::Internal::Send( std::string message, bool important ){
        if( !Connected() )
            return Error::NotConnected;
        if( important )
            ImportantMessages ++;
        //Necessary to avoid corrupting SendBuffer due to race conditions
        MyLock.Lock();
        SendBuffer.push_back( Message(message, important?CommStream::Internal::Message::IMPORTANT:CommStream::Internal::Message::NONE) );
        MyLock.Unlock();
        //Put the data on the line
        CommStream::ServiceSockets(this);
        return Error::None;
    }

    Error CommStream::Internal::Send( void* message, size_t len, bool important ){
        if( !Connected() )
            return Error::NotConnected;
        if( important )
            ImportantMessages ++;

        //Necessary to avoid corrupting SendBuffer due to race conditions
        MyLock.Lock();
        SendBuffer.push_back( Message((const char*)message, len, important?CommStream::Internal::Message::IMPORTANT:CommStream::Internal::Message::NONE) );
        MyLock.Unlock();
        //Put the data on the line
        CommStream::ServiceSockets(this);
        return Error::None;
    }

    Error CommStream::Internal::Receive( std::string &message ){
        if( !Connected() )
            return Error::NotConnected;
        //Collect any data pending on the network buffers
        CommStream::ServiceSockets(this);
        if( RecvBuffer.size() == 0 )
            return Error::NoData;
        //Necessary to avoid corrupting RecvBuffer due to race conditions
        MyLock.Lock();
        message = (const char*) RecvBuffer.front().msg.get();
        RecvBuffer.pop_front();
        MyLock.Unlock();
        return Error::None;
    }

    Error CommStream::Internal::Receive( char* message, size_t& len ){
        size_t len2 = len;
        len = 0;
        if( !Connected() )
            return Error::NotConnected;
        //Collect any data pending on the network buffers
        CommStream::ServiceSockets(this);
        if( RecvBuffer.size() == 0 )
            return Error::NoData;
        //Necessary to avoid corrupting RecvBuffer due to race conditions
        MyLock.Lock();
        //Fill the receive buffer with data from the buffers in RecvBuffer.
        //Continue filling message with data until it's full or no more messages
        //remain in RecvBuffer.
        while( len2 > 0 && RecvBuffer.size() > 0 ){
            const char* temp = RecvBuffer.front().msg.get();
            size_t length = RecvBuffer.front().length;
            int flags = RecvBuffer.front().flags;
            size_t copylength = length < len2 ? length : len2;

            memcpy( message, temp, copylength );
            length -= copylength;
            len2 -= copylength;
            message += copylength;
            len += copylength;
            //If data remains on the current recv buffer, then put it back in the queue
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

    Error CommStream::Internal::Encrypt( CommStream::Encryption type ){
        //TODO: implement
        return Error::InvalidScheme;
    }

    Error CommStream::Internal::RegisterCallback( std::function<void()> f ){
        //Register a function here that gets called every time data is received for this stream.
        callbacks.push_back( f );
        return Error::None;
    }

    Error CommStream::Internal::ClearCallbacks( ){
        callbacks.clear();
        return Error::None;
    }

    bool CommStream::Internal::HasData(){
        return RecvBuffer.size() > 0;
    }

    //Evaluate a socket for validity and grab data from the network buffers
    int CommStream::Internal::ServiceSocket( CommStream::Internal *ptr ){
            char recvbuf[NETBUFFERSIZE];
            Lock.Lock();
            int ret = 0;

            if( !ptr->isconnected ){
                ptr->Terminate();
                return -1;
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
                return -1;
            }
            Lock.Unlock();
            return ret;
    }

    int CommStream::Internal::ServiceSockets( CommStream::Internal *ptr ){
        Lock.Lock();
        if( !currentlyServicing ){
            currentlyServicing = true;
            int ret = 0;
            if( ptr != nullptr ){
                ret = ServiceSocket( ptr );
                if( ret == -1 ){
                    for( unsigned int i = 0; i < CommStreams.size(); ++i ){
                        if( CommStreams[i].Get() == ptr ){
                            CommStreams.erase( CommStreams.begin() + i );
                            break;
                        }
                    }
                }
            }
            else {
                for( unsigned int i = 0; i < CommStreams.size(); ++i ){
                    int amt = ServiceSocket( CommStreams[i].Get() );
                    if( amt >= 0 )
                        ret += amt;
                    if( amt == -1 ){
                        CommStreams.erase( CommStreams.begin() + i );
                        i--;
                    }
                }
            }

            currentlyServicing = false;
            Lock.Unlock();
            return ret;
        }
        Lock.Unlock();
        return 0;
    }

    int CommStream::Internal::SendBufferSize(){
        //Collect any data pending on the network buffers
        CommStream::ServiceSockets(this);

        return SendBuffer.size();
    }

    void CommStream::Internal::PushStream( CommStream topush ){
        Lock.Lock();
        topush.myInternal->ID = CurrentID++;
        CommStreams.push_back(topush);
        Lock.Unlock();
    }


    void CommStream::Internal::PopStream( CommStream *topush ){
        Lock.Lock();
        for( unsigned int i = 0; i < CommStreams.size(); ++i ){
            if( CommStreams[i].Get()->ID == topush->myInternal->ID ){
                CommStreams.erase( CommStreams.begin() + i );
                break;
            }
        }
        Lock.Unlock();
    }

    Mutex CommStream::Internal::Lock;
    std::vector<CommStream> CommStream::Internal::CommStreams;
    bool CommStream::Internal::currentlyServicing = false;
    int CommStream::Internal::CurrentID = 0;
}
