#include<map>
#include<string>
#include "Global/Error.hpp"
#include "CommStream/CommStream.hpp"
#include "Global/Thread.hpp"
#include "Telnet/Telnet.hpp"
#include <cctype>
#include <string.h>

#include<functional>

#include "Global/Strings.hpp"

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




    Telnet::TelnetSession::Screen::Screen( int Width, int Height, TelnetSession &Parent ) : myCursor( *this ), parent(Parent){
        width = Width;
        height = Height;
        supportsColor = parent.parent.SupportedTerms["DEFAULT"].Color;
        supportsMovement = parent.parent.SupportedTerms["DEFAULT"].ANSIEscape;
        myCursor.ShouldWrap( parent.parent.SupportedTerms["DEFAULT"].Wraps );

    }

    int Telnet::TelnetSession::Screen::Width(){
        return width;
    }

    int Telnet::TelnetSession::Screen::Height(){
        return height;
    }

    Error Telnet::TelnetSession::Screen::SetTerminal( std::string TerminalType ){
        supportsColor = parent.parent.SupportedTerms[TerminalType].Color;
        supportsMovement = parent.parent.SupportedTerms[TerminalType].ANSIEscape;
        myCursor.ShouldWrap( parent.parent.SupportedTerms[TerminalType].Wraps );
        return Error::None;
    }

    Error Telnet::TelnetSession::Screen::Resize( int w, int h ){
        if( w <= 0 || h <= 0 )
            return Error::InvalidSize;
        width = w;
        height = h;
        if( myCursor.X >= w )
            myCursor.X = w - 1;
        if( myCursor.X < 0 )
            myCursor.X = 0;
        if( myCursor.Y >= h )
            myCursor.Y = h - 1;
        if( myCursor.Y < 0 )
            myCursor.Y = 0;
        return Error::None;
    }



    void Telnet::TelnetSession::ReadStream(){
        lock.Lock();
        char buff[1002];
        buff[1000] = buff[1001] = '\0';

        size_t len = 1000;
        char* writeto = buff;
        if( bufferbacklog.length() != 0 ){
            memcpy( buff, &bufferbacklog[0], bufferbacklog.length() );
            len -= bufferbacklog.length();
            writeto += bufferbacklog.length();
            bufferbacklog = "";
        }

        while( len > 0 ){
            switch( stream.Receive( writeto, len ) ){
                case Error::NoData: len = 0; break;
                case Error::NotConnected: Disconnect( true ); len = 0; break;
                default: break;
            }
            if( len == 0 )
                break;
            unsigned int i = 0;
            char* start = buff;
            bool escaped = false;

            while( i < len ){
                if( !escaped && buff[i] == (char) Telnet::Commands::IAC ){
                    buff[i] = 0;
                    buffer += BufferToString( start, buff+i-start );
                    buff[i] = (char) Telnet::Commands::IAC;
                    start = buff+i;
                    if( buff[i+1] == (char) Telnet::Commands::IAC ){
                        escaped = true;
                        start ++;
                    }
                    else if( ParseCommand( start, len - i ) == Error::PartialData ){
                        bufferbacklog = BufferToString( start, len - i );
                        break;
                    }
                    i = start - buff;
                }
                else{
                    escaped = false;
                    i++;
                }
            }
            if( i == len ){
                buffer += BufferToString( start, buff+i-start );
            }
        }
        lock.Unlock();
    }

    Error Telnet::TelnetSession::ParseCommand( char*& cmd, size_t len ){
        if( *cmd != (char) Telnet::Commands::IAC )
            return Error::ParseFailure;
        char* start = cmd;
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
                    } break;
                    case Telnet::Commands::SUPPRESS_GO_AHEAD:{
                        SendCommand(Telnet::Commands::DO, Telnet::Commands::SUPPRESS_GO_AHEAD );
                    } break;
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
                        SendCommand(Telnet::Commands::WONT, Telnet::Commands::ECHO );
                    } break;
                    case Telnet::Commands::TERMINAL_TYPE:{
                        SendCommand(Telnet::Commands::WILL, Telnet::Commands::TERMINAL_TYPE );
                    } break;
                    case Telnet::Commands::SUPPRESS_GO_AHEAD:{
                        SendCommand(Telnet::Commands::WILL, Telnet::Commands::SUPPRESS_GO_AHEAD );
                    } break;
                    default:
                        SendCommand(Telnet::Commands::WONT, (Telnet::Commands)*cmd );
                        break;

                }
            } break;
            case Telnet::Commands::DONT:{
                if( --len == 0 ) {cmd = start; return Error::PartialData;}
                cmd++;
                switch((Telnet::Commands)*cmd){
                    case Telnet::Commands::NAWS:
                    case Telnet::Commands::ECHO:
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
                        if( *cmd == (char)Telnet::Commands::IAC ){
                            len--; if( len < 6 ) {cmd = start; return Error::PartialData;} cmd++;
                            if( *cmd != (char)Telnet::Commands::IAC ) break;
                        }
                        w += *cmd << 8;
                        cmd++;
                        if( *cmd == (char)Telnet::Commands::IAC ){
                            len--; if( len < 6 ) {cmd = start; return Error::PartialData;} cmd++;
                            if( *cmd != (char)Telnet::Commands::IAC ) break;
                        }
                        w += *cmd;
                        cmd++;
                        if( *cmd == (char)Telnet::Commands::IAC ){
                            len--; if( len < 6 ) {cmd = start; return Error::PartialData;} cmd++;
                            if( *cmd != (char)Telnet::Commands::IAC ) break;
                        }
                        h += *cmd << 8;
                        cmd++;
                        if( *cmd == (char)Telnet::Commands::IAC ){
                            len--; if( len < 6 ) {cmd = start; return Error::PartialData;} cmd++;
                            if( *cmd != (char)Telnet::Commands::IAC ) break;
                        }
                        h += *cmd;
                        myScreen.Resize( w, h );
                        cmd++;
                        do{
                            len--; if( len < 5 ) {cmd = start; return Error::PartialData;}
                            if( *cmd == (char)Telnet::Commands::IAC ){
                                len--; if( len < 5 ) {cmd = start; return Error::PartialData;}
                                cmd++;
                                if( *cmd == (char)Telnet::Commands::SE ){
                                    break;
                                }
                            }
                            cmd++;
                        }while( true );


                    } break;
                    case Telnet::Commands::TERMINAL_TYPE:{
                        if( --len == 0 ) {cmd = start; return Error::PartialData;}
                        cmd++;
                        if( *cmd == (char)Telnet::Commands::IS ){
                            if( --len == 0 ) {cmd = start; return Error::PartialData;}
                            cmd++;
                        }
                        std::string term = "";
                        while( true ){
                            while( *cmd != (char)Telnet::Commands::IAC ){
                                term += *cmd;
                                if( --len == 0 ) {cmd = start; return Error::PartialData;}
                                cmd++;
                            }
                            if( --len == 0 ) {cmd = start; return Error::PartialData;}
                            cmd++;
                            if( *cmd != (char)Telnet::Commands::IAC )
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
            } break;
            break;

        }
        return Error::None;
    }

    Telnet::TelnetSession::TelnetSession( CommStream s, Telnet &Parent ) : parent(Parent), stream(s), myScreen(80, 25, *this) {
        echos = false;
        s.RegisterCallback( std::bind(&Telnet::TelnetSession::ReadStream, this) );
        requestingTerminal = 0;
        bestTerminal = "DEFAULT";
        lastTerminal = "";
    }

    Error Telnet::TelnetSession::SendLine( std::string line ){
        return stream.Send( line );
    }

    Error Telnet::TelnetSession::SendChar( const char c ){
        std::string s = "";
        s += c;
        return stream.Send( s );
    }

    bool Telnet::TelnetSession::HasLine(){
        return buffer.find_first_of( "\n" ) != std::string::npos;
    }

    bool Telnet::TelnetSession::HasChar(){
        return buffer.length() > 0;
    }

    std::string Telnet::TelnetSession::ReadLine(){
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

    char Telnet::TelnetSession::ReadChar(){
        lock.Lock();
        char ret = '\0';
        if( buffer.length() > 0 ){
            ret = buffer[0];
            buffer.erase(0, 1);
        }
        lock.Unlock();
        return ret;
    }

    std::string Telnet::TelnetSession::PeekLine(){
        lock.Lock();
        auto pos = buffer.find_first_of( "\n" );
        std::string ret = "";
        if( pos != std::string::npos ){
            ret = buffer.substr( 0, pos + 1 );
        }
        lock.Unlock();
        return ret;
    }

    char Telnet::TelnetSession::PeekChar(){
        lock.Lock();
        char ret = '\0';
        if( buffer.length() > 0 ){
            ret = buffer[0];
        }
        lock.Unlock();
        return ret;
    }

    Error Telnet::TelnetSession::Disconnect( bool remoteFailure ){
        return stream.Disconnect(false);
    }

    Error Telnet::TelnetSession::SendCommand( Telnet::Commands cmd1, Telnet::Commands cmd2 ){
        std::string toSend = "";
        toSend += (char) Telnet::Commands::IAC;
        if( cmd1 != Telnet::Commands::NONE )
            toSend += (char) cmd1;
        if( cmd2 != Telnet::Commands::NONE )
            toSend += (char) cmd2;
        return SendLine( toSend );
    }

    Error Telnet::TelnetSession::SendSubnegotiation( Telnet::Commands cmd1, Telnet::Commands cmd2, char* data, size_t len ){
        std::string toSend = "";
        toSend += (char) Telnet::Commands::IAC;
        toSend += (char) Telnet::Commands::SB;
        if( cmd1 != Telnet::Commands::NONE )
            toSend += (char) cmd1;
        if( cmd2 != Telnet::Commands::NONE )
            toSend += (char) cmd2;
        if( len > 0 ){
            std::string toAdd = BufferToString( data, len );
            for( unsigned int i = 0; i < toAdd.length(); ++i ){
                if( toAdd[i] == (char)Telnet::Commands::IAC ){
                    std::string toInsert = "";
                    toInsert += (char)Telnet::Commands::IAC;
                    toAdd.insert( i++, toInsert );
                }
            }
            toSend += toAdd;
        }

        toSend += (char) Telnet::Commands::IAC;
        toSend += (char) Telnet::Commands::SE;
        return SendLine( toSend );
    }

    Error Telnet::TelnetSession::SendSubnegotiation( Telnet::Commands cmd1, char* data, size_t len ){
        return SendSubnegotiation( cmd1, Telnet::Commands::NONE, data, len );
    }





    void Telnet::ConnectionHandler( CommStream &cs, Telnet *parent ){

    }

    Telnet::Telnet(){

    }

    void Telnet::Listen( int port, std::function<void(TelnetSession)> callback ){

    }

    Error Telnet::ReadTerms( std::string fname ){
        FILE* f = fopen( fname.c_str(), "r" );
        if( f == NULL )
            return Error::FileNotFound;
        Terminal temp;
        bool readingterm = false;
        while( true ){
            char buffer[1000];

            fgets( buffer, 500, f );
            if( feof(f) )
                break;
            int strend = strlen( buffer );
            while(strend >= 0 && ( buffer[strend] == '\0' || isspace(buffer[strend]) ) ){
                buffer[strend--] = 0;
            }

            if( strlen( buffer ) == 0 ){
                if( readingterm ){
                    readingterm = false;
                    SupportedTerms[temp.Name] = temp;

                }
            }
            else{
                if( !readingterm ){
                    char* b = buffer;
                    while( *b != '\0' && isspace( *b ) )
                            b++;
                    strupr( b );
                    temp = Terminal();
                    temp.Name = b;
                    readingterm = true;
                    printf("%s\n", b);
                }
                else{
                    char bufferA[500];
                    char bufferB[500];

                    if( sscanf( buffer, "%s%s", bufferA, bufferB ) == 2 ){
                        char* bA = bufferA;
                        char* bB = bufferB;
                        while( *bA != '\0' && isspace( *bA ) )
                            bA++;
                        while( *bB != '\0' && isspace( *bB ) )
                            bB++;
                        strupr( bA );
                        strupr( bB );
                        bool affirmative = bB[0] == 'Y';

                        if( strcmp( bA, "COLOR:" ) == 0 ){
                            temp.Color = affirmative;
                        }
                        if( strcmp( bA, "WRAPS:" ) == 0 ){
                            temp.Wraps = affirmative;
                        }
                        if( strcmp( bA, "ANSI-ESCAPE:" ) == 0 ){
                            temp.ANSIEscape = affirmative;
                        }
                        if( strcmp( bA, "PREFERENCE:" ) == 0 ){
                            temp.Preference = atol( bB );
                        }
                        if( strcmp( bA, "INHERIT:" ) == 0 ){
                            std::string n = temp.Name;
                            temp = SupportedTerms[bB];
                            temp.Name = n;
                        }

                    }
                }
            }
        }
        if( readingterm ){
            readingterm = false;
            SupportedTerms[temp.Name] = temp;
        }
        return Error::None;
    }



}

