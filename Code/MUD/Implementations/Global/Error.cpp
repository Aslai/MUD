#include "Global/Error.hpp"
#include <cstdio>
namespace GlobalMUD{
    namespace ERROR{
        void Fail( int line, const char *file, const char *func, const char *msg ){
            printf( "Failure at\nLine: %d\nFile: %s\nFunc: %s\nMsg:  %s\n", line, file, func, msg );
        }
        void Test( const char *msg ){
            printf( "Testing %s...\n", msg );
        }
    }
}
