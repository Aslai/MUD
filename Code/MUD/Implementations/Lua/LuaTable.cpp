#include "Lua/Lua.hpp"

extern "C"{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}
#include<string>




Lua::Table::Table( lua_State *Lua, const char* name ) :
    globalname(name),
    ref(-1),
    L(Lua)
{
    p.L = L;
}
Lua::Table::Table( lua_State *Lua, int reference ) :
    globalname(""),
    ref(reference),
    L(Lua)
{
    p.L = L;
}

Lua::Table::Table( ) :
    globalname(""),
    ref(-1),
    L(0){
    p.L = L;
}
Lua::Table& Lua::Table::operator=(Lua::Table f){
    globalname = f.globalname;
    ref = f.ref;
    L = f.L;
    p.L = L;
    return *this;
}
size_t Lua::Table::Length(){
    Lua::GetValueToStack( L, globalname, ref );
    if( !lua_istable(L, -1) ){
        return 0;
    }
    size_t ret = lua_rawlen(L, -1);
    lua_pop(L,lua_gettop(L));
    return ret;
}
