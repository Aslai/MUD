#include "libmudlua/Lua/Lua.hpp"

extern "C"{
#include "lua/lua.h"
#include "lua/lualib.h"
#include "lua/lauxlib.h"
}
#include<string>
#include<cstdlib>
#include<cstring>
#include "libmudcommon/Strings/Strings.hpp"
#include "libmudcommon/Error/Error.hpp"

DLL_EXPORT Lua::Value::Value() : TableIndex(), TableKeys() {
    myType = Type::Nil;
    StringValue = "";
    NumberValue = 0;
    FunctionValue = nullptr;
}

DLL_EXPORT Lua::Value::Value(double value) : TableIndex(), TableKeys() {
    operator=(value);
}

DLL_EXPORT Lua::Value::Value(std::string value) : TableIndex(), TableKeys() {
    operator=(value);
}

DLL_EXPORT Lua::Value::Value(const Lua::Value& value) : TableIndex(), TableKeys() {
    myType = value.myType;
    TableIndex.clear();
    TableKeys.clear();
    TableIndex = value.TableIndex;
    TableKeys.insert( value.TableKeys.begin(), value.TableKeys.end() );

    StringValue = value.StringValue;
    NumberValue = value.NumberValue;
    FunctionValue = value.FunctionValue;
    LuaFunc = value.LuaFunc;
}

DLL_EXPORT Lua::Value::Value(Lua::Table value) : TableIndex(), TableKeys() {
    operator=(value);
}

DLL_EXPORT Lua::Value::Value(Lua::Function value) : TableIndex(), TableKeys() {
    operator=(value);
}

DLL_EXPORT bool Lua::Value::IsNumber(){
    return myType == Type::Number;
}

DLL_EXPORT bool Lua::Value::IsString(){
    return myType == Type::String;
}

DLL_EXPORT bool Lua::Value::IsTable(){
    return myType == Type::Table;
}

DLL_EXPORT bool Lua::Value::IsFunction(){
    return myType == Type::Function;
}

DLL_EXPORT bool Lua::Value::IsNil(){
    return myType == Type::Nil;
}

DLL_EXPORT double& Lua::Value::GetNumber(){
    return NumberValue;
}

DLL_EXPORT std::string& Lua::Value::GetString(){
    return StringValue;
}

DLL_EXPORT Lua::Function Lua::Value::GetFunction(){
    return LuaFunc;
}

DLL_EXPORT Lua::Value& Lua::Value::GetTable(std::string key){
    if( myType == Type::Nil )
        myType = Type::Table;
    return TableKeys[key];
}

DLL_EXPORT Lua::Value& Lua::Value::GetTable(unsigned int index){
    if( myType == Type::Nil )
        myType = Type::Table;
    if( index < TableIndex.size() )
        return TableIndex[index];
    return TableIndex[0];
}

DLL_EXPORT Lua::Value& Lua::Value::operator[](std::string key){
    return GetTable( key );
}

DLL_EXPORT Lua::Value& Lua::Value::operator[](unsigned int index){
    return GetTable( index );
}

DLL_EXPORT Lua::Value& Lua::Value::operator=(double value){
    myType = Type::Number;
    TableIndex.clear();
    TableKeys.clear();
    StringValue = "";
    NumberValue = value;
    FunctionValue = nullptr;
    return *this;
}

DLL_EXPORT Lua::Value& Lua::Value::operator=(std::string value){
    myType = Type::String;
    TableIndex.clear();
    TableKeys.clear();
    StringValue = value;
    NumberValue = 0;
    FunctionValue = nullptr;
    return *this;
}

DLL_EXPORT Lua::Value& Lua::Value::operator=(const Lua::Value value){
    myType = value.myType;
    TableIndex.clear();
    TableKeys.clear();
    TableIndex = value.TableIndex;
    TableKeys.insert( value.TableKeys.begin(), value.TableKeys.end() );

    StringValue = value.StringValue;
    NumberValue = value.NumberValue;
    FunctionValue = value.FunctionValue;
    LuaFunc = value.LuaFunc;
    return *this;
}

DLL_EXPORT Lua::Value& Lua::Value::operator=(Lua::Table value){
    myType = Type::Table;
    TableIndex.clear();
    TableKeys.clear();
    StringValue = "";
    NumberValue = 0;
    FunctionValue = nullptr;

    value.FillValue( *this );
    return *this;
}

DLL_EXPORT Lua::Value& Lua::Value::operator=(Lua::Function value){
    myType = Type::Function;
    TableIndex.clear();
    TableKeys.clear();
    StringValue = "";
    NumberValue = 0;
    FunctionValue = nullptr;
    LuaFunc = value;
    printf("\nVALUE.REF = %d\n\n", value.ref );
    return *this;
}

DLL_EXPORT Lua::Value::operator std::string(){
    switch( myType ){
        case Lua::Value::Type::Nil: return "Nil";
        case Lua::Value::Type::Number: return MUD::ToString( GetNumber() );
        case Lua::Value::Type::String: return MUD::ToString( GetString() );
        case Lua::Value::Type::Table:{
            std::string ret = "{\n";
            for( unsigned int i = 1; i < TableIndex.size(); ++i ){
                ret += TableIndex[i];
                ret += "\n";
            }
            auto iter = TableKeys.begin();
            while( iter != TableKeys.end() ){
                ret += (*iter).first;
                ret += " = ";
                ret += (*iter).second;
                ret += "\n";
                iter++;
            }
            ret += "}\n";
            return ret;
        }
        case Lua::Value::Type::Function: return "Function";
        default: return "Lua::Value";
    }
}
