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
                break;

        }
    }

    Telnet::TelnetSession::TelnetSession( CommStream s, Telnet &Parent ) : parent(Parent), stream(s), myScreen(80, 25, *this) {
        echos = false;
        s.RegisterCallback( std::bind(&Telnet::TelnetSession::ReadStream, this) );
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

    Error Telnet::TelnetSession::Disconnect( bool remoteFailure ){

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

    Error Telnet::TelnetSession::SendSubnegotiation( Telnet::Commands cmd, char* data, size_t len ){
        std::string toSend = "";
        toSend += (char) Telnet::Commands::IAC;
        toSend += (char) Telnet::Commands::SB;
        if( cmd != Telnet::Commands::NONE )
            toSend += (char) cmd;
        toSend += BufferToString( data, len );
        toSend += (char) Telnet::Commands::IAC;
        toSend += (char) Telnet::Commands::SE;
        return SendLine( toSend );
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
            //printf("%s\n", buffer);
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
                    //printf("%s\n", buffer);
                    //printf("%d\n", sscanf( buffer, "%s%s", bufferA, bufferB ) );
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
                        printf("%s\n", bA);
                        printf("%s\n", bB);

                        if( strcmp( bA, "COLOR:" ) == 0 ){
                            temp.Color = affirmative;
                            printf("JA");
                        }
                        if( strcmp( bA, "WRAPS:" ) == 0 ){
                            temp.Wraps = affirmative;
                        }
                        if( strcmp( bA, "ANSI-ESCAPE:" ) == 0 ){
                            temp.ANSIEscape = affirmative;
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

