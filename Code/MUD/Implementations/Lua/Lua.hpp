#ifndef KLUA_WRAPPER_HPP
#define KLUA_WRAPPER_HPP

extern "C"{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}
#include<string>
#include <vector>

#define RegisterFunction(a,b) funcreg<decltype(&a),a>(b,a)

class Lua{
    lua_State* L;
    int errorlevel;
    public:
    class Table;
    class Function;
    private:
    struct Stack{
        lua_State *L;int position;
        Stack();
        int trythrow(int idx);
        int lua_push( std::string s );
        int lua_push( const char* s );
        int lua_push( int s );
        int lua_push( unsigned int s );
        int lua_push( double s );
        int lua_push( void );
        int lua_push( void* s );
        std::string lua_ret( std::string, int pos = -100000 );
        const char* lua_ret( const char*, int pos = -100000 );
        int lua_ret( int , int pos = -100000 );
        double lua_ret( double, int pos = -100000 );
        void* lua_ret( void*, int pos = -100000 );
        Function lua_ret( Function, int pos = -100000 );
        Table lua_ret( Table, int pos = -100000 );
    };

    template <class RetType, class...args>
    struct LuaWrap{
        template<RetType (*fun)(args...)>
        static int luawrap(struct lua_State *Lua ){
            Stack p;
            p.L=Lua;
            p.lua_push( fun(  p.lua_ret( args() )... ) );
            return 1;
        }
    };

    static void report_errors(lua_State *L, int status);
    static void *l_alloc (void *ud, void *ptr, size_t osize, size_t nsize);
    static int panic( lua_State *l );
    struct WriteStruct{
        std::vector<char> buffer;
    };
    static int Writer (lua_State *L, const void* p, size_t sz, void* ud);
    static int Reader( lua_State *L, void *data, size_t *size);

    void Init();
    static bool GetValueToStack( lua_State *L, std::string globalid, int reference = -1 );

public:
    class Table{
        std::string globalname;
        Stack p;
        int ref;
        lua_State *L;
    public:
        Table( lua_State *Lua, const char* name );
        Table( lua_State *Lua, int reference );
        Table( );
        Table& operator=(Table f);
        size_t Length();

        template<typename... Args> inline void pass(Args...) {}

        template<class ReturnType, class T>
        ReturnType Get(T key){
            if( GetValueToStack( L, globalname, ref ) ){
                if (!lua_istable(L, -1)){
                    return ReturnType();
                }
                //Push our desired key to the stack
                p.lua_push( key );
                lua_gettable(L, -2);
                ReturnType ret = p.lua_ret( ReturnType(), -1 );

                lua_pop(L,lua_gettop(L));
                return ret;
            }
            return ReturnType();
        }
        template<class T, class V>
        bool Set(T key, V value){
            if( GetValueToStack( L, globalname, ref ) ){
                if (!lua_istable(L, -1)){
                    return false;
                }
                //Push our desired key to the stack
                p.lua_push( key );
                p.lua_push( value );
                lua_settable(L, -3);
                return true;
            }
            return false;
        }


    };

    class Function{
        std::string funcname;
        Stack p;
        int ref;
        lua_State *L;
    public:
        Function( lua_State *Lua, const char* name );
        Function( lua_State *Lua, int reference );
        Function( );
        Function& operator=(Function f);

        template<typename... Args> inline void pass(Args...)  const {}

        template<class ReturnType, class... Arguments>
        ReturnType Call(Arguments... a){
            p.position = 1; //Reset the position for calling another function
            if( GetValueToStack( L, funcname, ref ) ){
                if (!lua_isfunction(L, -1)){
                    return ReturnType();
                }
                pass(p.lua_push(a)...);

                 lua_pcall(L, sizeof...(Arguments), 1, 0);
                ReturnType ret = p.lua_ret(ReturnType(), -1);

                lua_pop(L,lua_gettop(L));
                return ret;
            }
            return ReturnType();
        }
    };

    template<class T, T f, class ReturnType, class... arg0>
    void funcreg(const char* name, ReturnType (*function)(arg0...)){
        lua_CFunction func = Lua::LuaWrap<ReturnType, arg0...>::template luawrap<f>;
        lua_register(L, name, func);
    }


    void Load(std::string block, std::string name, int flags);
    void Load(std::vector<char> block, std::string name, int flags);
    Lua( std::string block, std::string name, int flags );
    Lua( std::vector<char> block, std::string name, int flags );
    Lua();
    ~Lua();
    std::vector<char> Dump() const;
    Function GetFunction( const char* name ) const;
    void Run();

    template <class type>
    void Set( std::string name, type value ){
        Stack p;
        p.L=L;
        p.lua_push( value );
        lua_setglobal (L, name.c_str() );
    }

    template <class type>
    type Get( std::string name ) const{
        Stack p;
        p.L=L;
        lua_getglobal (L, name.c_str() );
        return (type) p.lua_ret( type(), -1 );
    }
};

#endif
