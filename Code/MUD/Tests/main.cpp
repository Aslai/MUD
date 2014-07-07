#include<cstdio>
#include "Tester.hpp"
#include "CommStream/CommStream.hpp"
#include "CommStream/Cipher.hpp"
#include "Thread/Thread.hpp"
#include "HTTPd/HTTPd.hpp"
#include "Error/Error.hpp"
#include "Memory/Stream.hpp"

#include <random>
#include <cstdio>
#include<functional>
#include "Telnet/Telnet.hpp"
#include "Strings/Strings.hpp"

#include "Lua/Lua.hpp"

using namespace GlobalMUD;

void TNet(GlobalMUD::Telnet::TelnetSession t);
void TNet(GlobalMUD::Telnet::TelnetSession t){
    t.Screen().Cursor().Hide();
    printf("a");
    while( t.Connected() ){
        Thread::Sleep(100);
        t.Screen().Clear();

        t.Screen().Cursor().MoveTo( 1, t.Screen().Height() );
        t.Screen().SetColor( Telnet::Color::Bright_White, Telnet::Color::Green );
        t.SendLine( StringRepeat(" ", t.Screen().Width() ) );

        t.Screen().Cursor().MoveTo( 1, t.Screen().Height() );
        t.SendLine( StringFormat("Terminal Size: %d\t%d", t.Screen().Width(), t.Screen().Height() ) );

        std::string title = "Telnet Test";
        t.Screen().SetColor( Telnet::Color::Bright_White, Telnet::Color::Black );
        t.Screen().Cursor().MoveTo( ( t.Screen().Width() - title.length() ) / 2, 3 );


        t.SendLine( title );

    }
}

int main(){
    Lua l;
    l.Load( "                   \
            Stats = {           \
                \"hp\",         \
                \"mp\",         \
                \"fatigue\",    \
                \"str\",        \
                \"dex\",        \
                \"wis\",        \
                \"int\",        \
                \"luk\"         \
            }                   \
           ", "main", 0 );

    l.Run();
    Lua::Table table = l.Get<Lua::Table>("Stats");
    for( size_t i = 1; i < table.Length(); ++i ){
        Print( table.Get<std::string>(i), "\n" );
    }
    return 0;



    GlobalMUD::Telnet t;
    t.ReadTerms( "terminals.txt" );
    t.Listen( 45141, TNet );
    return 0;

    Test<GlobalMUD::Thread>();
    Test<GlobalMUD::Ciphers::Cipher>();
    Test<GlobalMUD::Ciphers::XOR>();
    Test<GlobalMUD::CommStream>();
    Test<GlobalMUD::HTTPd>();
}
