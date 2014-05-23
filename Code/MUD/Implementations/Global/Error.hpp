#ifndef MUD_GLOBAL_ERROR_HPP
#define MUD_GLOBAL_ERROR_HPP
#undef ERROR
#define ErrorRoot 0
#define FAIL(msg) {GlobalMUD::ERROR::Fail( __LINE__, __FILE__, __func__, msg );}
#define TEST(msg) {GlobalMUD::ERROR::Test(msg);}
#define ASSERT(expr) {if(!(expr)){GlobalMUD::ERROR::Fail( __LINE__, __FILE__, __func__, "Assertion Failure" ); return false;};}
#define TRACE printf("TRACE: %d %s\n", __LINE__, __FILE__ )

namespace GlobalMUD{
    typedef const unsigned int Error;
    namespace ERROR{
        Error None = ErrorRoot + 0;
        void Fail( int line, const char *file, const char *func, const char *msg );
        void Test( const char *msg );
    }
}

#endif // MUD__GLOBAL_ERROR_HPP
