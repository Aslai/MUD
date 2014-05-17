#ifndef MUD_COMMSTREAM_COMMSTREAM_HPP
#define MUD_COMMSTREAM_COMMSTREAM_HPP

#include <string>
#include <vector>
#include <deque>
#include <memory>
#include <utility>
#define SOCKET unsigned int

#include "Global/Error.hpp"
#include "CommStream/Cipher.hpp"
#define NETBUFFERSIZE 1000

#undef ErrorRoot
#define ErrorRoot 100
namespace GlobalMUD{
    namespace ERROR{
        Error ConnectionFailure = ErrorRoot + 0;
        Error NegotiationFailure = ErrorRoot + 1;
        Error InvalidScheme = ErrorRoot + 2;
        Error InvalidHost = ErrorRoot + 3;
        Error ConnectionRefused = ErrorRoot + 4;
        Error ImportantOperation = ErrorRoot + 5;
        Error BindFailure = ErrorRoot + 6;
        Error PartialMessage = ErrorRoot + 7;
        Error NotConnected = ErrorRoot + 8;
        Error ListenFailure = ErrorRoot + 9;
        Error NoData = ErrorRoot + 10;
        Error SocketFailure = ErrorRoot + 11;

    }
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
        ReceiveType MyReceiveType;
        Encryption MyEncryption;
        Ciphers::Cipher* MyCipher;
        SOCKET Connection;
        std::deque<Message> SendBuffer;
        int ImportantMessages;
        std::deque<Message> RecvBuffer;
        std::string RecvLinesBuffer;
        bool isconnected;
        void Terminate();
        void PushData( char* data, size_t len );
        bool CanSend();
        Message GetSend();

        public:
        CommStream( ReceiveType rectype = LINES );
        ~CommStream();
        Error Connect( std::string address, int port );
        bool Connected( );
        Error Disconnect( bool force = false );
        Error Listen( int port, void(*func)(CommStream stream, void* data), void* data = 0 );
        Error Listen( std::string address, int port, void(*func)(CommStream stream, void* data), void* data = 0 );
        Error Send( std::string message, bool important = false );
        Error Receive( std::string &message );
        Error Encrypt( Encryption type );
        #ifdef RunUnitTests
        static bool RunTests();
        #endif
        static void ServiceSockets();
        static std::vector<CommStream*> CommStreams;
    };
}
#undef SOCKET

#endif
