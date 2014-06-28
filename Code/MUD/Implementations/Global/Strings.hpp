#ifndef MUD_GLOBAL_STRINGS_HPP
#define MUD_GLOBAL_STRINGS_HPP

#include<string>
#include<string.h>
#include<stdio.h>


namespace GlobalMUD{
    std::string BufferToString( const char* buffer, size_t len );
    std::string StringToUpper( std::string str );

    template<class... Args>
    std::string StringFormat( std::string format, Args... args ){
        size_t len = snprintf( NULL, 0, format.c_str(), args...);
        std::string ret;
        ret.resize(len);
        ret.reserve(len+1);
        snprintf( &ret[0], len+1, format.c_str(), args...);
        return ret;
    }

    void strupr( char *str );
}

#endif
