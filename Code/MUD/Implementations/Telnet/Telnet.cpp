#include<map>
#include<string>
#include "Global/Error.hpp"
#include "CommStream/CommStream.hpp"
#include "Global/Thread.hpp"
#include "Telnet/Telnet.hpp"
#include <cctype>
#include <string.h>

#include<functional>

static void strupr( char* str ){
    while( *str ){
        *str = toupper( *str );
        str++;
    }
}


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
        if( w <= 0 || y <= 0 )
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



    Telnet::TelnetSession::TelnetSession( CommStream s, Telnet &Parent ) : parent(Parent), myScreen(80, 25, *this), stream(s){
        echos = false;
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

