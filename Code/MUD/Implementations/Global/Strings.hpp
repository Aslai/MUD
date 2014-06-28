#ifndef MUD_GLOBAL_STRINGS_HPP
#define MUD_GLOBAL_STRINGS_HPP

#include<string>

namespace GlobalMUD{
    std::string BufferToString( const char* buffer, size_t len );
    std::string StringToUpper( std::string str );

    void strupr( char *str );
}

#endif
