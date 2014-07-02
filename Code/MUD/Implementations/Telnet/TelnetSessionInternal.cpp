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
#include "Memory/Stream.hpp"

namespace GlobalMUD{
    void Telnet::TelnetSessionInternal::ReadStream(){
        lock.Lock();
        unsigned char buff[1002];
        buff[1000] = buff[1001] = '\0';

        size_t len = 1000;
        unsigned char* writeto = buff;
        if( bufferbacklog.length() != 0 ){
            memcpy( buff, &bufferbacklog[0], bufferbacklog.length() );
            len -= bufferbacklog.length();
            writeto += bufferbacklog.length();
            bufferbacklog = "";
        }

        while( len > 0 ){
            switch( stream.Receive( (char*)writeto, len ) ){
                case Error::NoData: len = 0; break;
                case Error::NotConnected: Disconnect( true ); len = 0; break;
                default: break;
            }

            if( len == 0 )
                break;
            unsigned int i = 0;
            unsigned char* start = buff;
            bool escaped = false;

            while( i < len ){
                if( !escaped && buff[i] == (unsigned char) Telnet::Commands::IAC ){
                    buff[i] = 0;
                    buffer += BufferToString( (char*)start, buff+i-start );
                    buff[i] = (unsigned char) Telnet::Commands::IAC;
                    start = buff+i;
                    if( buff[i+1] == (unsigned char) Telnet::Commands::IAC ){
                        escaped = true;
                        start ++;

                    }
                    else{
                        Error e = ParseCommand( start, len - i );
                        if( e == Error::PartialData ){
                            bufferbacklog = BufferToString( (char*)start, len - i );

                            break;
                        }
                        if( e == Error::ParseFailure ){

                        }

                    }
                    i = start - buff;
                }
                else{
                    escaped = false;
                    i++;
                }
            }
            if( i >= len ){
                std::string s = BufferToString( (char*)start, buff+i-start );
                buffer += s;
                if( echos == true )
                    SendLine( s );
            }
        }
        lock.Unlock();
    }

    Error Telnet::TelnetSessionInternal::ParseCommand( unsigned char*& cmd, size_t len ){
        if( *cmd != (unsigned char) Telnet::Commands::IAC )
            return Error::ParseFailure;
        unsigned char* start = cmd;
        if( --len == 0 ) return Error::PartialData;
        cmd++;
        switch((Telnet::Commands)*cmd){
            case Telnet::Commands::NOP:
            case Telnet::Commands::IP: //Not implemented
            case Telnet::Commands::AO: //Not implemented
            case Telnet::Commands::DataMark: //Not implemented
            case Telnet::Commands::BRK: //Not implemented
            case Telnet::Commands::GA: //Not implemented
            default:

            break;

            case Telnet::Commands::EC:{
                auto pos = buffer.find_last_of("\n");
                if( buffer.length() > 0 && (pos == std::string::npos || pos != buffer.length()-1) ){
                    buffer.resize( buffer.length() - 1);
                }
            } break;
            case Telnet::Commands::EL:{
                auto pos = buffer.find_last_of("\n");
                if( buffer.length() > 0 && pos == std::string::npos ){
                    buffer = "";
                }
                else if( buffer.length() > 0 ){
                    buffer = buffer.substr( 0, pos + 1 );
                }
            } break;
            case Telnet::Commands::AYT:
                SendLine("Yes, I am here.");
                break;
            case Telnet::Commands::IAC:
                break;
            case Telnet::Commands::WILL:{
                if( --len == 0 ) {cmd = start; return Error::PartialData;}
                cmd++;
                switch((Telnet::Commands)*cmd){
                    case Telnet::Commands::NAWS:{
                        SendCommand(Telnet::Commands::DO, Telnet::Commands::NAWS );
                    } break;
                    case Telnet::Commands::ECHO:{
                        SendCommand(Telnet::Commands::DONT, Telnet::Commands::ECHO );
                    } break;
                    case Telnet::Commands::TERMINAL_TYPE:{
                        SendCommand(Telnet::Commands::DO, Telnet::Commands::TERMINAL_TYPE );
                        SendSubnegotiation( Telnet::Commands::TERMINAL_TYPE, Telnet::Commands::SEND );
                    } break;
                    case Telnet::Commands::SUPPRESS_GO_AHEAD:{
                        SendCommand(Telnet::Commands::DO, Telnet::Commands::SUPPRESS_GO_AHEAD );
                    } break;/**/
                    default:
                        SendCommand(Telnet::Commands::DONT, (Telnet::Commands)*cmd );
                        break;
                }
            } break;
            case Telnet::Commands::WONT:{
                if( --len == 0 ) {cmd = start; return Error::PartialData;}
                cmd++;
                switch((Telnet::Commands)*cmd){
                    case Telnet::Commands::NAWS:
                    case Telnet::Commands::ECHO:
                    case Telnet::Commands::TERMINAL_TYPE:
                    case Telnet::Commands::SUPPRESS_GO_AHEAD:
                    default:
                        break;
                }
            } break;
            case Telnet::Commands::DO:{
                if( --len == 0 ) {cmd = start; return Error::PartialData;}
                cmd++;
                switch((Telnet::Commands)*cmd){
                    case Telnet::Commands::NAWS:{
                        SendCommand(Telnet::Commands::WILL, Telnet::Commands::NAWS );
                    } break;
                    case Telnet::Commands::ECHO:{
                        SendCommand(Telnet::Commands::WILL, Telnet::Commands::ECHO );
                        echos = true;
                    } break;
                    case Telnet::Commands::TERMINAL_TYPE:{
                        SendCommand(Telnet::Commands::WILL, Telnet::Commands::TERMINAL_TYPE );

                    } break;
                    case Telnet::Commands::SUPPRESS_GO_AHEAD:{
                        SendCommand(Telnet::Commands::WILL, Telnet::Commands::SUPPRESS_GO_AHEAD );
                    } break;/**/
                    default:
                        SendCommand(Telnet::Commands::WONT, (Telnet::Commands)*cmd );
                        break;

                }
            } break;
            case Telnet::Commands::DONT:{
                if( --len == 0 ) {cmd = start; return Error::PartialData;}
                cmd++;
                switch((Telnet::Commands)*cmd){
                    case Telnet::Commands::ECHO:{
                        echos = false;
                        SendCommand(Telnet::Commands::WONT, (Telnet::Commands)*cmd );
                    } break;
                    case Telnet::Commands::NAWS:

                    case Telnet::Commands::TERMINAL_TYPE:
                    case Telnet::Commands::SUPPRESS_GO_AHEAD:
                    default:
                        SendCommand(Telnet::Commands::WONT, (Telnet::Commands)*cmd );
                        break;
                }
            } break;
            case Telnet::Commands::SB:{
                if( --len == 0 ) {cmd = start; return Error::PartialData;}
                cmd++;
                Telnet::Commands thisCmd = (Telnet::Commands)*cmd;
                switch( thisCmd ){
                    case Telnet::Commands::NAWS:{
                        if( len < 6 ) {cmd = start; return Error::PartialData;}
                        cmd++;
                        int w, h;
                        w = h = 0;
                        if( *cmd == (unsigned char)Telnet::Commands::IAC ){
                            len--; if( len < 6 ) {cmd = start; return Error::PartialData;} cmd++;
                            if( *cmd != (unsigned char)Telnet::Commands::IAC ) break;
                        }
                        w += *cmd << 8;
                        cmd++;
                        if( *cmd == (unsigned char)Telnet::Commands::IAC ){
                            len--; if( len < 6 ) {cmd = start; return Error::PartialData;} cmd++;
                            if( *cmd != (unsigned char)Telnet::Commands::IAC ) break;
                        }
                        w += *cmd;
                        cmd++;
                        if( *cmd == (unsigned char)Telnet::Commands::IAC ){
                            len--; if( len < 6 ) {cmd = start; return Error::PartialData;} cmd++;
                            if( *cmd != (unsigned char)Telnet::Commands::IAC ) break;
                        }
                        h += *cmd << 8;
                        cmd++;
                        if( *cmd == (unsigned char)Telnet::Commands::IAC ){
                            len--; if( len < 6 ) {cmd = start; return Error::PartialData;} cmd++;
                            if( *cmd != (unsigned char)Telnet::Commands::IAC ) break;
                        }
                        h += *cmd;
                        Screen.Resize( w, h );
                        cmd++;
                        do{
                            len--; if( len < 5 ) {cmd = start; return Error::PartialData;}
                            if( *cmd == (unsigned char)Telnet::Commands::IAC ){
                                len--; if( len < 5 ) {cmd = start; return Error::PartialData;}
                                cmd++;
                                if( *cmd == (unsigned char)Telnet::Commands::SE ){
                                    break;
                                }
                            }
                            cmd++;
                        }while( true );


                    } break;
                    case Telnet::Commands::TERMINAL_TYPE:{
                        if( --len == 0 ) {cmd = start; return Error::PartialData;}
                        cmd++;
                        if( *cmd == (unsigned char)Telnet::Commands::IS ){
                            if( --len == 0 ) {cmd = start; return Error::PartialData;}
                            cmd++;
                        }
                        std::string term = "";
                        while( true ){
                            while( *cmd != (unsigned char)Telnet::Commands::IAC ){
                                term += *cmd;
                                if( --len == 0 ) {cmd = start; return Error::PartialData;}
                                cmd++;
                            }
                            if( --len == 0 ) {cmd = start; return Error::PartialData;}
                            cmd++;
                            if( *cmd != (unsigned char)Telnet::Commands::IAC )
                            break;
                        }

                        switch(requestingTerminal){
                        case 0:{
                            if( term == lastTerminal ){
                                requestingTerminal = 1;
                            }
                            lastTerminal = term;
                            if( parent.SupportedTerms[StringToUpper(bestTerminal)].Preference <=
                                    parent.SupportedTerms[StringToUpper(term)].Preference ){
                                bestTerminal = term;
                            }
                            SendSubnegotiation( Telnet::Commands::TERMINAL_TYPE, Telnet::Commands::SEND );
                        } break;
                        case 1:{
                            if( term == bestTerminal ){
                                requestingTerminal = 2;
                                printf("|%s|\n", term.c_str());
                                Screen.SetTerminal( StringToUpper( term ) );
                            }
                            else
                                SendSubnegotiation( Telnet::Commands::TERMINAL_TYPE, Telnet::Commands::SEND );
                        } break;
                        default:
                            break;
                        }
                    } break;
                    default:
                        break;
                }
                cmd++;
            } break;
            break;

        }
        return Error::None;
    }

    Telnet::TelnetSessionInternal::TelnetSessionInternal( CommStream s, Telnet &Parent ) : parent(Parent), stream(s), lock(), Screen(80, 25, *this) {
        echos = false;
        requestingTerminal = 0;
        bestTerminal = "DEFAULT";
        lastTerminal = "";
        buffer = "";
        bufferbacklog = "";
        stream.RegisterCallback( std::bind(&Telnet::TelnetSessionInternal::ReadStream, this) );
        SendCommand( Telnet::Commands::DO, Telnet::Commands::TERMINAL_TYPE );
    }
    Telnet::TelnetSessionInternal::~TelnetSessionInternal( ){
        stream.ClearCallbacks();
        Disconnect(false);
    }


    Error Telnet::TelnetSessionInternal::SendLine( std::string line ){
        return stream.Send( line );
    }

    Error Telnet::TelnetSessionInternal::SendChar( const char c ){
        std::string s = "";
        s += c;
        return stream.Send( s );
    }

    bool Telnet::TelnetSessionInternal::HasLine(){
        //return false;
        ScopedMutex m( lock );
        m.Lock();
        return buffer.find_first_of( "\n" ) != std::string::npos;
    }

    bool Telnet::TelnetSessionInternal::HasChar(){
        ScopedMutex m( lock );
        m.Lock();
        return buffer.length() > 0;
    }

    std::string Telnet::TelnetSessionInternal::ReadLine(){
        lock.Lock();
        auto pos = buffer.find_first_of( "\n" );
        std::string ret = "";
        if( pos != std::string::npos ){
            ret = buffer.substr( 0, pos + 1 );
            buffer = buffer.substr( pos + 1 );
        }
        lock.Unlock();
        return ret;
    }

    char Telnet::TelnetSessionInternal::ReadChar(){
        lock.Lock();
        char ret = '\0';
        if( buffer.length() > 0 ){
            ret = buffer[0];
            buffer.erase(0, 1);
        }
        lock.Unlock();
        return ret;
    }

    std::string Telnet::TelnetSessionInternal::PeekLine(){
        lock.Lock();
        auto pos = buffer.find_first_of( "\n" );
        std::string ret = "";
        if( pos != std::string::npos ){
            ret = buffer.substr( 0, pos + 1 );
        }
        lock.Unlock();
        return ret;
    }

    char Telnet::TelnetSessionInternal::PeekChar(){
        lock.Lock();
        char ret = '\0';
        if( buffer.length() > 0 ){
            ret = buffer[0];
        }
        lock.Unlock();
        return ret;
    }

    Error Telnet::TelnetSessionInternal::Disconnect( bool remoteFailure ){
        return stream.Disconnect(false);
    }

    Error Telnet::TelnetSessionInternal::SendCommand( Telnet::Commands cmd1, Telnet::Commands cmd2 ){
        std::string toSend = "";
        toSend += (unsigned char) Telnet::Commands::IAC;
        if( cmd1 != Telnet::Commands::NONE )
            toSend += (unsigned char) cmd1;
        if( cmd2 != Telnet::Commands::NONE )
            toSend += (unsigned char) cmd2;

        return SendLine( toSend );
    }

    Error Telnet::TelnetSessionInternal::SendSubnegotiation( Telnet::Commands cmd1, Telnet::Commands cmd2, char* data, size_t len ){
        std::string toSend = "";
        toSend += (unsigned char) Telnet::Commands::IAC;
        toSend += (unsigned char) Telnet::Commands::SB;
        if( cmd1 != Telnet::Commands::NONE )
            toSend += (unsigned char) cmd1;
        if( cmd2 != Telnet::Commands::NONE )
            toSend += (unsigned char) cmd2;
        if( len > 0 ){
            std::string toAdd = BufferToString( data, len );
            for( unsigned int i = 0; i < toAdd.length(); ++i ){
                if( toAdd[i] == (unsigned char)Telnet::Commands::IAC ){
                    std::string toInsert = "";
                    toInsert += (unsigned char)Telnet::Commands::IAC;
                    toAdd.insert( i++, toInsert );
                }
            }
            toSend += toAdd;
        }

        toSend += (char) Telnet::Commands::IAC;
        toSend += (char) Telnet::Commands::SE;
        return SendLine( toSend );
    }

    Error Telnet::TelnetSessionInternal::SendSubnegotiation( Telnet::Commands cmd1, char* data, size_t len ){
        return SendSubnegotiation( cmd1, Telnet::Commands::NONE, data, len );
    }

    bool Telnet::TelnetSessionInternal::Connected(){
        return stream.Connected();
    }
}

