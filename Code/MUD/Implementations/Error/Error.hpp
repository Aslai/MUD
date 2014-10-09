#ifdef ERROR
#undef ERROR
#endif
#ifndef MUD_ERROR_ERROR_HPP
#define MUD_ERROR_ERROR_HPP

#include <string>
#include <map>

#define FAIL(msg) {GlobalMUD::ERROR::Fail( __LINE__, __FILE__, __func__, msg );}
#define TEST(msg) {GlobalMUD::ERROR::Test(msg);}
#define ASSERT(expr) {if(!(expr)){GlobalMUD::ERROR::Fail( __LINE__, __FILE__, __func__, "Assertion Failure" ); return false;};}
#define TRACE printf("TRACE: %d %s\n", __LINE__, __FILE__ )

namespace GlobalMUD{
    //typedef const unsigned int Error;
    enum class Error {
        None = 0,
        PartialMessage,
        NotConnected,
        MountExists,
        NoData,
        EndOfLine,
        EndOfFile,
        NotAnError,
        AlreadyEnabled,
        PartialData,
        Bad = 0x1000,
        ParseFailure,
        CryptoFailure,
        ConnectionFailure,
        ConnectionRefused,
        ImportantOperation,
        NegotiationFailure,
        InvalidScheme,
        InvalidHost,
        BindFailure,
        ListenFailure,
        SocketFailure,
        FileNotFound,
        MountFailure,
        InvalidSize,
        Timeout,
        LuaError,
        Unsupported,
        OutOfBounds,
        Fatal = 0x10000,
        OutOfMemory
    };

    namespace ERROR{
        extern std::string ErrorStrings[];
        std::string ToString(Error e);
        void Fail( int line, const char *file, const char *func, const char *msg );
        void Test( const char *msg );

    }
}

#endif // MUD__GLOBAL_ERROR_HPP
