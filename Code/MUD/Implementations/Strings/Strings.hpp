#ifndef MUD_STRINGS_STRINGS_HPP
#define MUD_STRINGS_STRINGS_HPP

#include<string>
#include<string.h>
#include<stdio.h>


namespace GlobalMUD{
    std::string BufferToString( const char* buffer, size_t len );
    std::string StringToUpper( std::string str );
    std::string StringToLower(std::string in);
    std::string StringFromUInt(unsigned int i);
    unsigned int HashString(std::string v);

    template<class... Args>
    std::string StringFormat( std::string format, Args... args ){
        size_t len = snprintf( NULL, 0, format.c_str(), args...);
        std::string ret;
        ret.resize(len);
        ret.reserve(len+1);
        snprintf( &ret[0], len+1, format.c_str(), args...);
        return ret;
    }

    static const int wrapPoint = ((2 << (sizeof(unsigned int)*4)) - 1);
    constexpr unsigned int HashString(const char* v, unsigned int inNum = 27487){
        return (
                    (
                        v[1] != '\0' ?
                            ( v[0] + HashString( v+1, (v[0]*v[0]*inNum) % wrapPoint ) )
                        :
                            v[0]
                    ) * inNum
                )
                % wrapPoint;
    }

    void strupr( char *str );
    std::string StringRepeat(std::string str, int amt);

}

#endif
