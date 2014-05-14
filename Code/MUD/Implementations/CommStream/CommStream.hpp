#ifndef MUD_COMMSTREAM_COMMSTREAM_HPP
#define MUD_COMMSTREAM_COMMSTREAM_HPP

#include <string>
#include <vector>
#include <deque>
#define SOCKET unsigned int

#include "Global/Error.hpp"
#include "CommStream/Cipher.hpp"
#undef ErrorRoot
#define ErrorRoot 100
namespace GlobalMUD{
    namespace ERROR{
        DEF_ERROR(ConnectionFailure,0);
        DEF_ERROR(NegotiationFailure,1);
        DEF_ERROR(InvalidScheme,2);
        DEF_ERROR(InvalidHost,3);
        DEF_ERROR(ConnectionRefused,4);
        DEF_ERROR(ImportantOperation,5);
        DEF_ERROR(BindFailure,6);
        DEF_ERROR(PartialMessage,7);
        DEF_ERROR(NotConnected,8);
        DEF_ERROR(ListenFailure,9);
        DEF_ERROR(NoData,10);
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
            std::string msg;
            int flags;
        };
        ReceiveType MyReceiveType;
        Encryption MyEncryption;
        Ciphers::Cipher* MyCipher;
        SOCKET Connection;
        std::deque<Message> SendBuffer;
        int ImportantMessages;
        std::deque<Message> RecvBuffer;
        bool isconnected;

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
