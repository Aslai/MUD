#include<map>
#include<string>
#include "Error/Error.hpp"
#include "CommStream/CommStream.hpp"
#include "Thread/Thread.hpp"
#include "Telnet/Telnet.hpp"
#include <cctype>
#include <string.h>

#include<functional>

#include "Strings/Strings.hpp"

namespace GlobalMUD{
    Telnet::TelnetSession::TelnetSession( CommStream s, Telnet &Parent ) : internal( new TelnetSessionInternal( s, Parent ) ) {

    }
    Telnet::TelnetSession::~TelnetSession( ){

    }

    Telnet::TheScreen& Telnet::TelnetSession::Screen(){
        return internal->Screen;
    }

    Error Telnet::TelnetSession::SendLine( std::string line ){
        return internal->SendLine( line );
    }

    Error Telnet::TelnetSession::SendChar( const char c ){
        return internal->SendChar( c );
    }

    bool Telnet::TelnetSession::HasLine(){
        return internal->HasLine();
    }

    bool Telnet::TelnetSession::HasChar(){
        return internal->HasChar();
    }

    std::string Telnet::TelnetSession::ReadLine(){
        return internal->ReadLine();
    }

    char Telnet::TelnetSession::ReadChar(){
        return internal->ReadChar();
    }

    std::string Telnet::TelnetSession::PeekLine(){
        return internal->PeekLine();
    }

    char Telnet::TelnetSession::PeekChar(){
        return internal->PeekChar();
    }

    Error Telnet::TelnetSession::Disconnect( bool remoteFailure ){
        return internal->Disconnect( remoteFailure );
    }

    Error Telnet::TelnetSession::SendCommand( Telnet::Commands cmd1, Telnet::Commands cmd2 ){
        return internal->SendCommand( cmd1, cmd2 );
    }

    Error Telnet::TelnetSession::SendSubnegotiation( Telnet::Commands cmd1, Telnet::Commands cmd2, char* data, size_t len ){
        return internal->SendSubnegotiation( cmd1, cmd2, data, len );
    }

    Error Telnet::TelnetSession::SendSubnegotiation( Telnet::Commands cmd1, char* data, size_t len ){
        return internal->SendSubnegotiation( cmd1, data, len );
    }

    bool Telnet::TelnetSession::Connected(){
        return internal->Connected();
    }
}

