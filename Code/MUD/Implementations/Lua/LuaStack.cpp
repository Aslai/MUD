#include "Lua/Lua.hpp"

extern "C"{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}
#include<string>
#include "Error/Error.hpp"

Lua::Stack::Stack(){
    position = 1;
}

int Lua::Stack::trythrow(int idx){
    int argc = lua_gettop(L);
    if( lua_isnil(L, idx) ) return -1;
    //printf("IDX: %d | ARGC: %d\n", idx, argc );
    if( argc + idx < 0 || idx > argc || idx == 0 )
        return -1;
    return 0;
}
int Lua::Stack::lua_push( std::string s ){
    lua_pushstring( L, s.c_str() );
    return 0;
}

int Lua::Stack::lua_push( const char* s ){
    lua_pushstring( L, s );
    return 0;
}

int Lua::Stack::lua_push( int s ){
    lua_pushinteger( L, s );
    return 0;
}

int Lua::Stack::lua_push( unsigned int s ){
    lua_pushunsigned( L, s );
    return 0;
}

int Lua::Stack::lua_push( double s ){
    lua_pushnumber( L, s );
    return 0;
}

int Lua::Stack::lua_push( void ){
    return 0;
}

int Lua::Stack::lua_push( void* s ){
    lua_pushnumber( L, (long long)s );
    return 0;
}

std::string Lua::Stack::lua_ret( std::string, int pos ){
    if( pos == -100000 ) pos = position++;
    if( trythrow(pos) == 0 )
        return luaL_checkstring( L, pos );
    return "";
}

const char* Lua::Stack::lua_ret( const char*, int pos ){
    if( pos == -100000 ) pos = position++;
    if( trythrow(pos) == 0 )
        return luaL_checkstring( L, pos );
    return "";
}

int Lua::Stack::lua_ret( int , int pos ){
    if( pos == -100000 ) pos = position++;
    if( trythrow(pos) == 0 ){
        try{
            return luaL_checkinteger( L, pos );
        }
        catch( GlobalMUD::Error e ){
            return 0;
        }
    }
    return 0;
}

double Lua::Stack::lua_ret( double, int pos ){
    if( pos == -100000 ) pos = position++;
    if( trythrow(pos) == 0 )
        try{
            return luaL_checknumber( L, pos );
        }
        catch( GlobalMUD::Error e ){
            return 0;
        }
    return 0;
}

void* Lua::Stack::lua_ret( void*, int pos ){
    if( pos == -100000 ) pos = position++;
    if( trythrow(pos) == 0 ){
        try{
            return (void*)luaL_checkint( L, pos );
        }
        catch( GlobalMUD::Error e ){
            return 0;
        }
    }
    return 0;
}

Lua::Function Lua::Stack::lua_ret( Lua::Function, int pos ){
    if( pos == -100000 ) pos = position++;
    if( trythrow(pos) == 0 ){
        int f = luaL_ref(L, LUA_REGISTRYINDEX);
        Function ret(L, f);
        return ret;
    }
    return Function(L, "");
}

Lua::Table Lua::Stack::lua_ret( Lua::Table, int pos ){
    if( pos == -100000 ) pos = position++;
    if( trythrow(pos) == 0 ){
        int f = luaL_ref(L, LUA_REGISTRYINDEX);
        Table ret(L, f);
        return ret;
    }
    return Table(L, "");
}
