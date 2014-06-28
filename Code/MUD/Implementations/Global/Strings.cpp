#include "Global/Strings.hpp"
#include <cstring>

namespace GlobalMUD{
    void strupr( char* str ){
        while( *str ){
            *str = toupper( *str );
            str++;
        }
    }

    std::string BufferToString( const char* buffer, size_t len ){
        std::string ret;
        ret.resize( len );
        memcpy( &ret[0], buffer, len);
        return ret;
    }
}
