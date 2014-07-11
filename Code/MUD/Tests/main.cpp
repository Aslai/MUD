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

HTTPd::HTTPResponse func( HTTPd::HTTPResponse headers, HTTPd& parent ){
    HTTPd::HTTPResponse response;
    response.status = 200;
    auto iter = headers.gets.begin();
    std::string content = "{";
    while( iter != headers.gets.end() ){
        if( content != "{" )
            content += ",";
        auto pair = *iter;
        content += StringFormat( " %s = %s", pair.first, pair.second );
        iter++;
    }
    content += " }";
    response.SetContent( content );
    return response;
}

HTTPd::HTTPResponse my404( HTTPd::HTTPResponse headers, HTTPd& parent ){
    HTTPd::HTTPResponse response;
    response.status = 200;
    response.SetContent( "DANGER!!! DANGER!!! 404!!! 404!!! DANGER!!! DANGER!!!" );
    return response;
}

int testfunc(int a){
    Print("\n\nWOAH ", a, "\n\n\n");
    return 1337;
}

int main(){
    HTTPd h("any", 45141);
    h.MountDirectory( "/", "Editor/" );
    h.Run();
    return 0;
    Lua::Script script;
    //script.LoadString( "                    \
            Player = {                      \
                name = \"Mister Awesome\",  \
                hp = 450,                   \
                mp = 120,                   \
                fatigue = 300,              \
                str = 16,                   \
                dex = 12,                   \
                wis = 21,                   \
                int = 22,                   \
                luk = 15,                   \
                attributes =                \
                    {                       \
                        \"Cartographer\",   \
                        \"Poison immunity\" \
                    }                       \
            }                               \
            io.write(Test.Poot)\
            io.write(Test.Funky(9))\
           ", "main" );

    script.LoadFile( "TestCampaign/campaign.lua", "Campaign" );

    Lua l(script);
    l.Run();
    Lua::Value table = l.Get<Lua::Value>("Campaign");
    Print( table );

    /*PrintFormat(    "Name: %s\nHP: %f\nMP: %f\nFatigue: %f\n...\n\n"
                    "Attributes: {\n%s,\n%s\n}\n",
            table["name"].GetString(),
            table["hp"].GetNumber(),
            table["mp"].GetNumber(),
            table["fatigue"].GetNumber(),
            table["attributes"][1].GetString(),
            table["attributes"][2].GetString()
            );*/
    //table.iterate<char>();

    /*for( size_t i = 1; i < table.Length(); ++i ){
        Print( table.Get<std::string>(i), "\n" );
    }

    Print("\n\nTesting table setting and getting by key\n\n");
    table.Set( "p", 42 );
    l.GetFunction( "Printer" ).Call();*/
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
