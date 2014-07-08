#include "Lua/Lua.hpp"

extern "C"{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}
#include<string>
#include<cstdlib>
#include<cstring>
#include "Error/Error.hpp"

Lua::Value::Value() : TableIndex(), TableKeys() {
    myType = Type::Nil;
    StringValue = "";
    NumberValue = 0;
}

Lua::Value::Value(double value) : TableIndex(), TableKeys() {
    operator=(value);
}

Lua::Value::Value(std::string value) : TableIndex(), TableKeys() {
    operator=(value);
}

Lua::Value::Value(const Lua::Value& value) : TableIndex(), TableKeys() {
    myType = value.myType;
    TableIndex.clear();
    TableKeys.clear();
    TableIndex = value.TableIndex;
    TableKeys.insert( value.TableKeys.begin(), value.TableKeys.end() );

    StringValue = value.StringValue;
    NumberValue = value.NumberValue;
}

Lua::Value::Value(Lua::Table value) : TableIndex(), TableKeys() {
    operator=(value);
}

bool Lua::Value::IsNumber(){
    return myType == Type::Number;
}

bool Lua::Value::IsString(){
    return myType == Type::String;
}

bool Lua::Value::IsTable(){
    return myType == Type::Table;
}

bool Lua::Value::IsNil(){
    return myType == Type::Nil;
}

double& Lua::Value::GetNumber(){
    return NumberValue;
}

std::string& Lua::Value::GetString(){
    return StringValue;
}

Lua::Value& Lua::Value::GetTable(std::string key){
    return TableKeys[key];
}

Lua::Value& Lua::Value::GetTable(unsigned int index){
    if( index < TableIndex.size() )
        return TableIndex[index];
    return TableIndex[0];
}

Lua::Value& Lua::Value::operator[](std::string key){
    return GetTable( key );
}

Lua::Value& Lua::Value::operator[](unsigned int index){
    return GetTable( index );
}

Lua::Value& Lua::Value::operator=(double value){
    myType = Type::Number;
    TableIndex.clear();
    TableKeys.clear();
    StringValue = "";
    NumberValue = value;
    return *this;
}

Lua::Value& Lua::Value::operator=(std::string value){
    myType = Type::String;
    TableIndex.clear();
    TableKeys.clear();
    StringValue = value;
    NumberValue = 0;
    return *this;
}

Lua::Value& Lua::Value::operator=(const Lua::Value value){
    myType = value.myType;
    TableIndex.clear();
    TableKeys.clear();
    TableIndex = value.TableIndex;
    TableKeys.insert( value.TableKeys.begin(), value.TableKeys.end() );

    StringValue = value.StringValue;
    NumberValue = value.NumberValue;
    return *this;
}

Lua::Value& Lua::Value::operator=(Lua::Table value){
    value.FillValue( *this );
    return *this;
}
