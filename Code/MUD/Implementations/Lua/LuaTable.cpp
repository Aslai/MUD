#include "Lua/Lua.hpp"

extern "C"{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}
#include<string>
#include "Error/Error.hpp"

void Lua::Table::FillValue( Value& toFill ){
    if( GetValueToStack( L, globalname, ref ) ){
        if (lua_istable(L, -1))
        {
            lua_pushvalue(L, -1);
            lua_pushnil(L);
            while (lua_next(L, -2))
            {
                lua_pushvalue(L, -2);

                Lua::Value value;

                if( lua_istable(L, -2) ){
                    value = p.lua_ret( Lua::Table(), -2 );
                } else{
                    double num = lua_tonumber( L, -2 );
                    std::string str = p.lua_ret( std::string(), -2 );

                    if( num > .0000001 || num < -.000000001 ){
                        value = num;
                    }
                    else{
                        value = str;
                    }
                }

                int num = lua_tonumber(L, -1);
                double num2 = lua_tonumber(L, -1);
                if( num != 0 ){
                    if( toFill.TableIndex.size() <= num2 ){
                        toFill.TableIndex.resize(num+1);
                    }
                    toFill.TableIndex[num] = value;
                }
                else{
                    toFill.TableKeys[p.lua_ret( std::string(), -1 )] = value;
                }
                lua_pop(L, 2);
            }
            lua_pop(L, 1);
        }
        lua_pop(L, 1);
    }

}

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
