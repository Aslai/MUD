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

using namespace GlobalMUD;

void TNet(GlobalMUD::Telnet::TelnetSession t){
    t.Screen().Cursor().Hide();

    while( t.Connected() ){
        Thread::Sleep(100);
        t.Screen().Clear();

        t.Screen().SetColor( Telnet::Color::Bright_White, Telnet::Color::Green );
        t.Screen().Cursor().MoveTo( 1, t.Screen().Height() );
        t.SendLine( StringRepeat(" ", t.Screen().Width() ) );

        t.Screen().Cursor().MoveTo( 1, t.Screen().Height() );
        t.SendLine( StringFormat("Terminal Size: %d\t%d", t.Screen().Width(), t.Screen().Height() ) );

        std::string title = "Telnet Test";
        t.Screen().Cursor().MoveTo( ( t.Screen().Width() - title.length() ) / 2, 3 );

        t.Screen().SetColor( Telnet::Color::Default, Telnet::Color::Default );
        t.SendLine( title );

    }
}

int main(){
    #ifdef _WIN32
    WSADATA globalWSAData;
    WSAStartup( MAKEWORD(2, 2), &globalWSAData );
    #endif

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
