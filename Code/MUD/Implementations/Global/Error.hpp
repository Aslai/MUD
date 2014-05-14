#ifndef MUD_GLOBAL_ERROR_HPP
#define MUD_GLOBAL_ERROR_HPP

#define ErrorRoot 0
#define FAIL(msg) {GlobalMUD::ERROR::Fail( __LINE__, __FILE__, __func__, msg );}
#define TEST(msg) {GlobalMUD::ERROR::Test(msg);}
#define ASSERT(expr) {if(!expr){GlobalMUD::ERROR::Fail( __LINE__, __FILE__, __func__, msg );};}
#define DEF_ERROR(name,id) const unsigned int name = ErrorRoot + id

namespace GlobalMUD{
    typedef unsigned int Error;
    namespace ERROR{
        DEF_ERROR(None,0);
        void Fail( int line, const char *file, const char *func, const char *msg );
        void Test( const char *msg );
    }
}

#endif // MUD__GLOBAL_ERROR_HPP
