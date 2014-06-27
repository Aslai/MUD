#include<map>
#include<string>
#include "Global/Error.hpp"
#include "CommStream/CommStream.hpp"
#include "Global/Thread.hpp"
#include "Telnet/Telnet.hpp"

#include<functional>


namespace GlobalMUD{

    Telnet::TelnetSession::Screen::Cursor::Cursor( Screen& screen ) : myScreen(screen) {
        X = Y = 0;
        wraps = false;
        BGColor = Telnet::Color::Black;
        FGColor = Telnet::Color::White;
    }

    Error Telnet::TelnetSession::Screen::Cursor::ShouldWrap( bool shouldWrap ){
        wraps = shouldWrap;
        return Error::None;
    }

    Error Telnet::TelnetSession::Screen::Cursor::Advance( int amount ){
        if( X < myScreen.Width() - 1 ){
            X++;
        }
        else {
            if( wraps ){
                CarriageReturn();
                LineFeed();
            }
        }
        return Error::None;
    }

    Error Telnet::TelnetSession::Screen::Cursor::LineFeed( int amount ){
        if( Y < myScreen.Height() - 1 )
            Y++;
        return Error::None;
    }

    Error Telnet::TelnetSession::Screen::Cursor::CarriageReturn(){
        X = 0;
        return Error::None;
    }

    Error Telnet::TelnetSession::Screen::Cursor::SetColor( Color foreground, Color background ){
        BGColor = background;
        FGColor = foreground;
        return Error::None;
    }

    Error Telnet::TelnetSession::Screen::Cursor::MoveTo( int x, int y ){
        if( X < 0 || X >= myScreen.Width() || Y < 0 || Y >= myScreen.Height() ){
            return Error::OutOfBounds;
        }
        X = x;
        Y = y;
        return Error::None;
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

