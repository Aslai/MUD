#include "Global/Error.hpp"
#include <cstdio>
namespace GlobalMUD{
    namespace ERROR{
        std::string ErrorStrings[] =    {
                                "No error (Error::None)",
                                "Out of memory (Error::OutOfMemory)",
                                "Parse failure (Error::ParseFailure)",
                                "Unspecified crypto failure (Error::CryptoFailure)",
                                "Unspecified connection failure (Error::ConnectionFailure)",
                                "Failed to negotiate (Error::NegotiationFailure)",
                                "Invalid crypto scheme specified (Error::InvalidScheme)",
                                "Unable to lookup the specified host (Error::InvalidHost)",
                                "Connection was refused (Error::ConnectionRefused)",
                                "Important operation in progress (Error::ImportantOperation)",
                                "Failed to bind to specified port (Error::BindFailure)",
                                "Partial message received (Error::PartialMessage)",
                                "No active connection (Error::NotConnected)",
                                "Failed to listen (Error::ListenFailure)",
                                "No data (Error::NoData)",
                                "Unspecified socket failure (Error::SocketFailure)",
                                "File not found (Error::FileNotFound)",
                                "Failed to mount (Error::MountFailure)",
                                "Mount already exists (Error::MountExists)",
                                "Timeout period exceeded (Error::Timeout)",
                                "End of line (Error::EndOfLine)",
                                "End of file (Error::EndOfFile)",

                                "Not an Error (Error::NotAnError)"
                            };


        void Fail( int line, const char *file, const char *func, const char *msg ){
            printf( "Failure at\nLine: %d\nFile: %s\nFunc: %s\nMsg:  %s\n", line, file, func, msg );
        }
        void Test( const char *msg ){
            printf( "Testing %s...\n", msg );
        }
        std::string ToString(Error e){
            static_assert( ((int)Error::NotAnError)+1 == sizeof(ErrorStrings)/sizeof(ErrorStrings[0]), "Error list size mismatch" );
            return ErrorStrings[(int)e];
        }
    }
}
