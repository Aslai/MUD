#include "CommStream.hpp"
#include "Global/Error.hpp"

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
    CommStream::Message::Message(std::string message, int flagss ){
        msg = message;
        flags = flagss;
    }

    CommStream::CommStream( ReceiveType rectype ){
        #ifdef _WIN32
        WSADATA globalWSAData;
        WSAStartup( MAKEWORD(2, 2), &globalWSAData );
        #endif
        MyReceiveType = rectype;
        MyEncryption = NONE;
        MyCipher = new Ciphers::Cipher();
        isconnected = false;
        ImportantMessages = 0;
    }

    CommStream::~CommStream(){
        if( MyCipher )
            delete MyCipher;
    }

    Error CommStream::Connect( std::string address, int port ){
        //Look up the host / resolve IP
        hostent* host = gethostbyname( address.c_str() );
        if( host == 0 ) return ERROR::InvalidHost;

        //Connect the socket
        SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        static u_long iMode=1;
        ioctlsocket(sock,FIONBIO,&iMode);
        if( sock == (unsigned)SOCKET_ERROR ) return ERROR::ConnectionFailure;
        sockaddr_in clientService;
        clientService.sin_family = AF_INET;
        clientService.sin_port = htons( port );

        int result;
        for( int i = 0; host->h_addr_list[i] != NULL && i < 10; ++i ){
            clientService.sin_addr.s_addr = *((unsigned long long*) host->h_addr_list[i]);
            result = connect( sock, (sockaddr*) &clientService, sizeof(clientService));
            if( result != SOCKET_ERROR ) break;
        }
        if( result < 0 ) return ERROR::ConnectionFailure;
        Connection = sock;
        return ERROR::None;
    }

    bool CommStream::Connected( ){
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

    Error CommStream::Disconnect( bool force ){
        if( !force && ImportantMessages > 0 )
            return ERROR::ImportantOperation;
        if( closesocket( Connection ) == SOCKET_ERROR ){
            return ERROR::NotConnected;
        }
        return ERROR::None;
    }

    Error CommStream::Listen( int port, void(*func)(CommStream stream, void* data), void* data ){
        return Listen( "localhost", port, func, data );
    }

    Error CommStream::Listen( std::string address, int port, void(*func)(CommStream stream, void* data), void* data ){
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
        while(true){
            asock = accept( sock, 0, 0 );
            static u_long iMode=1;
            ioctlsocket(asock,FIONBIO,&iMode);
            if( asock == (unsigned)SOCKET_ERROR ){
                break;
            }
            CommStream toPass(MyReceiveType);
            toPass.Connection = asock;
            toPass.isconnected = true;
            func( toPass, data );
        }
        return ERROR::ConnectionFailure;
    }

    Error CommStream::Send( std::string message, bool important ){
        if( !isconnected )
            return ERROR::NotConnected;
        if( important )
            ImportantMessages ++;
        SendBuffer.push_back( Message(message, important?CommStream::Message::IMPORTANT:CommStream::Message::NONE) );
        return ERROR::None;
    }

    Error CommStream::Receive( std::string &message ){
        if( !isconnected )
            return ERROR::NotConnected;
        if( RecvBuffer.size() == 0 )
            return ERROR::NoData;
        message = RecvBuffer.front().msg;
        RecvBuffer.pop_front();
        return ERROR::None;
    }

    Error CommStream::Encrypt( Encryption type ){
        return ERROR::InvalidScheme;
    }

    #ifdef RunUnitTests
    bool CommStream::RunTests(){
        TEST("GlobalMUD::CommStream");
        FAIL("GlobalMUD::CommStream not yet implemented.")
        return false;
    }
    #endif

    void CommStream::ServiceSockets(){

    }

    std::vector<CommStream*> CommStream::CommStreams;
}
