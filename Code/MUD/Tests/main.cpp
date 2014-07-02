#include<cstdio>
#include "Tester.hpp"
#include "CommStream/CommStream.hpp"
#include "CommStream/Cipher.hpp"
#include "Thread/Thread.hpp"
#include "HTTPd/HTTPd.hpp"
#include "Error/Error.hpp"

#include <random>
#include <cstdio>
#include<functional>
#include "Telnet/Telnet.hpp"
#include "Strings/Strings.hpp"

using namespace GlobalMUD;

void TNet(GlobalMUD::Telnet::TelnetSession t){
    //return;
    t.SendANSICode( Telnet::ANSICodes::CursorHide );
    while( t.Connected() ){
        Thread::Sleep(100);
        if( t.SendANSICode( Telnet::ANSICodes::EraseDisplay, 2 ) == Error::Unsupported ) printf("FUCK");;
        t.SendANSICode( Telnet::ANSICodes::CursorPosition, t.Screen().Height(), 1 );

        t.Screen().SetColor( Telnet::Color::Bright_White, Telnet::Color::Green );
        t.SendANSICode( Telnet::ANSICodes::CursorPosition, t.Screen().Height(), 1 );
        std::string block = "          ";
        for( ; block.length() < (unsigned)t.Screen().Width(); block = block + block ){}
        block = block.substr( 0, t.Screen().Width() );
        t.SendLine( block );

        t.SendANSICode( Telnet::ANSICodes::CursorPosition, t.Screen().Height(), 1 );
        t.SendLine( StringFormat("Terminal Size: %d\t%d", t.Screen().Width(), t.Screen().Height() ) );

        std::string title = "Telnet Test";
        t.SendANSICode( Telnet::ANSICodes::CursorPosition, 3, ( t.Screen().Width() - title.length() ) / 2 );
        t.Screen().SetColor( Telnet::Color::Default, Telnet::Color::Default );
        t.SendLine( title );

    }
printf("k");
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
    //Test<GlobalMUD::HTTPd>();
}
