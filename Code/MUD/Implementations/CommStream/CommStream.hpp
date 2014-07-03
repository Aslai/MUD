#ifndef MUD_COMMSTREAM_COMMSTREAM_HPP
#define MUD_COMMSTREAM_COMMSTREAM_HPP

#include <string>
#include <vector>
#include <deque>
#include <memory>
#include "Memory/RefCounter.hpp"
#include <utility>
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

#include "Error/Error.hpp"
#include "Thread/Mutex.hpp"
#include "CommStream/Cipher.hpp"
#include "Thread/Thread.hpp"
#define NETBUFFERSIZE 1000


namespace GlobalMUD{

    class CommStream{
    public:
        enum Encryption{
            NONE = 0,
            XOR = 1
        };
        enum ReceiveType{
            BINARY = 0,
            LINES = 1
        };
    private:
        class Internal{
            friend CommStream;
            struct Message{
                enum FLAGS{
                    NONE = 0,
                    IMPORTANT = 1
                };
                Message(std::string message, int flagss );
                Message(const char* message, size_t len, int flagss );

                std::shared_ptr<char> msg;
                size_t length;
                int flags;
            };
            CommStream::ReceiveType MyReceiveType;
            CommStream::Encryption MyEncryption;
            Ciphers::Cipher* MyCipher;
            SOCKET Connection;
            std::deque<Message> SendBuffer;
            int ImportantMessages;
            std::deque<Message> RecvBuffer;
            std::string RecvLinesBuffer;
            bool aborted;
            bool isconnected;
            bool isServer;
            static bool currentlyServicing;
            unsigned int connecting;
            bool transmitting;
            void Terminate();
            void PushData( char* data, size_t len );
            bool CanSend();
            Message GetSend();
            Mutex MyLock;
            int disconnecting;
            int ID;
            std::vector<std::function<void()>> callbacks;

            public:
            Internal( CommStream::ReceiveType rectype = CommStream::LINES );
            ~Internal();
            Error Connect( std::string address, int port );
            bool Connected( );
            Error Disconnect( bool force = false );
            Error Listen( int port, std::function<void(CommStream)> );
            Error ListenOn( std::string address, int port, std::function<void(CommStream)> );
            Error Send( std::string message, bool important = false );
            Error Send( void* message, size_t len, bool important = false );
            Error Receive( std::string &message );
            Error Receive( char* message, size_t& len );
            Error Encrypt( CommStream::Encryption type );
            Error RegisterCallback( std::function<void()> );
            Error ClearCallbacks( );
            bool HasData();
            int SendBufferSize();

            static int ServiceSockets(Internal *ptr = nullptr);
            static int ServiceSocket(Internal *ptr = nullptr);

            static void PushStream( CommStream topush );
            static void PopStream( CommStream * id );

            static int CurrentID;

            static Mutex Lock;
            static std::vector<CommStream> CommStreams;
        };
        friend Internal;

        void UseSocket(SOCKET sock);
        void Terminate();
        void PushData( char* data, size_t len );
        bool CanSend();
        Internal* Get();
        SOCKET GetConnection();
        RefCounter<Internal> myInternal;


    public:
        CommStream& operator=(CommStream other);
        CommStream( ReceiveType rectype = LINES );
        ~CommStream();
        Error Connect( std::string address, int port );
        bool Connected( );
        Error Disconnect( bool force = false );
        Error Listen( int port, std::function<void(CommStream)> );
        Error ListenOn( std::string address, int port, std::function<void(CommStream)> );
        Error Send( std::string message, bool important = false );
        Error Send( void* message, size_t len, bool important = false );
        Error SendFile( std::string path, bool important = false );
        Error Receive( std::string &message );
        Error Receive( char* message, size_t& len );
        Error Encrypt( Encryption type );
        Error RegisterCallback( std::function<void()> );
        Error ClearCallbacks( );
        bool HasData();

        void Flush();
        int SendBufferSize();
        #ifdef RunUnitTests
        static bool RunTests();
        #endif
        static int ServiceSockets(Internal *ptr = nullptr);


    };
}
#undef SOCKET

#endif
