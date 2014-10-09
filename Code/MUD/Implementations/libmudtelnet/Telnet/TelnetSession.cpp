#include<map>
#include<string>
#include "libmudcommon/Error/Error.hpp"
#include "libmudcommon/CommStream/CommStream.hpp"
#include "libmudcommon/Thread/Thread.hpp"
#include "libmudtelnet/Telnet/Telnet.hpp"
#include <cctype>
#include <string.h>

#include<functional>

#include "libmudcommon/Strings/Strings.hpp"

namespace MUD{
    //This entire object is just a wrapper that keeps a refcounted pointer to TelnetSessionInternal, and allows
    //user interaction via these wrapper functions.

    DLL_EXPORT Telnet::TelnetSession::TelnetSession( CommStream s, Telnet &Parent ) : internal( new TelnetSessionInternal( s, Parent ) ) {

    }
    DLL_EXPORT Telnet::TelnetSession::~TelnetSession( ){

    }

    DLL_EXPORT Telnet::TheScreen& Telnet::TelnetSession::Screen(){
        return internal->Screen;
    }

    DLL_EXPORT Error Telnet::TelnetSession::SendLine( std::string line ){
        return internal->SendLine( line );
    }

    DLL_EXPORT Error Telnet::TelnetSession::SendChar( const char c ){
        return internal->SendChar( c );
    }

    DLL_EXPORT bool Telnet::TelnetSession::HasLine(){
        return internal->HasLine();
    }

    DLL_EXPORT bool Telnet::TelnetSession::HasChar(){
        return internal->HasChar();
    }

    DLL_EXPORT std::string Telnet::TelnetSession::ReadLine(){
        return internal->ReadLine();
    }

    DLL_EXPORT char Telnet::TelnetSession::ReadChar(){
        return internal->ReadChar();
    }

    DLL_EXPORT std::string Telnet::TelnetSession::PeekLine(){
        return internal->PeekLine();
    }

    DLL_EXPORT char Telnet::TelnetSession::PeekChar(){
        return internal->PeekChar();
    }

    DLL_EXPORT Error Telnet::TelnetSession::Disconnect( bool remoteFailure ){
        return internal->Disconnect( remoteFailure );
    }

    DLL_EXPORT Error Telnet::TelnetSession::SendCommand( Telnet::Commands cmd1, Telnet::Commands cmd2 ){
        return internal->SendCommand( cmd1, cmd2 );
    }

    DLL_EXPORT Error Telnet::TelnetSession::SendSubnegotiation( Telnet::Commands cmd1, Telnet::Commands cmd2, char* data, size_t len ){
        return internal->SendSubnegotiation( cmd1, cmd2, data, len );
    }

    DLL_EXPORT Error Telnet::TelnetSession::SendSubnegotiation( Telnet::Commands cmd1, char* data, size_t len ){
        return internal->SendSubnegotiation( cmd1, data, len );
    }

    DLL_EXPORT bool Telnet::TelnetSession::Connected(){
        return internal->Connected();
    }
}

