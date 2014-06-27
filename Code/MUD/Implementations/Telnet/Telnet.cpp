#include<map>
#include<string>
#include "Global/Error.hpp"
#include "CommStream/CommStream.hpp"
#include "Global/Thread.hpp"
#include "Telnet/Telnet.hpp"

#include<functional>


namespace GlobalMUD{

    Telnet::TelnetSession::Screen::Cursor::Cursor( Screen& screen ) : myScreen(screen) {

    }

    void Telnet::TelnetSession::Screen::Cursor::ShouldWrap( bool shouldWrap ){

    }

    Error Telnet::TelnetSession::Screen::Cursor::Advance( int amount ){

    }

    Error Telnet::TelnetSession::Screen::Cursor::LineFeed( int amount ){

    }

    Error Telnet::TelnetSession::Screen::Cursor::CarriageReturn(){

    }

    void Telnet::TelnetSession::Screen::Cursor::SetColor( Color foreground, Color background ){

    }





    Telnet::TelnetSession::Screen::Screen( int Width, int Height ) : myCursor( *this ){

    }

    int Telnet::TelnetSession::Screen::Width(){

    }

    int Telnet::TelnetSession::Screen::Height(){

    }





    Telnet::TelnetSession::TelnetSession(){

    }

    Error Telnet::TelnetSession::SendLine( std::string line ){

    }

    Error Telnet::TelnetSession::SendChar( const char c ){

    }

    bool Telnet::TelnetSession::HasLine(){

    }

    bool Telnet::TelnetSession::HasChar(){

    }

    std::string Telnet::TelnetSession::ReadLine(){

    }

    char Telnet::TelnetSession::ReadChar(){

    }

    std::string Telnet::TelnetSession::PeekLine(){

    }

    char Telnet::TelnetSession::PeekChar(){

    }





    void Telnet::ConnectionHandler( CommStream &cs, Telnet *parent ){

    }

    Telnet::Telnet(){

    }

    void Telnet::Listen( int port, std::function<void(TelnetSession)> callback ){

    }



}

