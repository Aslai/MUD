#include "Lua/Lua.hpp"

extern "C"{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}
#include<string>


DLL_EXPORT Lua::Script::Script(){
    Title = "";
}

DLL_EXPORT Lua::Script::~Script(){

}

DLL_EXPORT void Lua::Script::LoadFile( std::string fname, std::string title ){
    Lua l;
    l.Load(fname);
    Title = title;
    Data = l.Dump();
}

DLL_EXPORT void Lua::Script::LoadString( std::string script, std::string title ){
    Lua l;
    l.Load(script, title, 0);
    Title = title;
    Data = l.Dump();
}

DLL_EXPORT void Lua::Script::SetTitle( std::string title ){
    Title = title;
}

DLL_EXPORT std::string Lua::Script::GetTitle(){
    return Title;
}

