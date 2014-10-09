#include "Strings/Strings.hpp"
#include <cstring>
#include <cctype>
#include<cstdio>

namespace MUD{
    DLL_EXPORT void strupr( char* str ){
        while( *str ){
            *str = toupper( *str );
            str++;
        }
    }

    DLL_EXPORT std::string BufferToString( const char* buffer, size_t len ){
        std::string ret;
        ret.resize( len );

        memcpy( &ret[0], buffer, len);
        return ret;
    }

    DLL_EXPORT std::string StringToUpper( std::string str ){
        for( unsigned int i = 0; i < str.length(); ++i ){
            str[i] = toupper(str[i]);
        }
        return str;
    }

    DLL_EXPORT std::string StringToLower(std::string in){
        for( size_t i = 0; i < in.size(); ++i ){
            in[i] = tolower(in[i]);
        }
        return in;
    }
    DLL_EXPORT std::string StringFromUInt(unsigned int i){
        char s[300];
        snprintf(s, 29, "%u", i );
        return s;
    }

    DLL_EXPORT unsigned int HashString(std::string v){
        return HashString(v.c_str() );
    }


    DLL_EXPORT std::string StringRepeat(std::string str, int amt){
        std::string ret = "";
        for( ; amt-->0; ret += str ){}
        return ret;
    }
}
