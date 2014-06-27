#ifndef MUD_TELNET_TELNET_HPP
#define MUD_TELNET_TELNET_HPP


#include<map>
#include<string>
#include "Global/Error.hpp"
#include "CommStream/CommStream.hpp"
#include "Global/Thread.hpp"

#include<functional>


namespace GlobalMUD{
    class Telnet{
    public:
        enum class Commands{
            //Begin RFC 854 commands//
            SE = 240,   //Subnegotiation end
            NOP,        //No Op
            DataMark,   //Data mark
            BRK,        //Break
            IP,         //Interrupt process
            AO,         //Abort output
            AYT,        //Are you there?
            EC,         //Erase character
            EL,         //Erase line
            GA,         //Go ahead
            SB,         //Subnegotiation begin
            WILL,
            WONT,
            DO,
            DONT,
            IAC,        //Command code / escape code
            IS = 0,
            SEND = 1,

            //Begin RFC 1073 commands//
            NAWS = 31,  //Negotiate about window size
                        //(IAC SB NAWS [16 bit value] [16 bit value] IAC SE)
                        //Values in network order. Default is undefined.

            //Begin RFC 857 commands//
            ECHO = 1,   //Start/stop echoing. Default is no echo.

            //Begin RFC 1091 commands//
            TERMINAL_TYPE = 24 //

        };
        enum class Color{
            Black = 0,
            Red,
            Green,
            Yellow,
            Blue,
            Magenta,
            Cyan,
            White,
            Bright_Black,
            Bright_Red,
            Bright_Green,
            Bright_Yellow,
            Bright_Blue,
            Bright_Magenta,
            Bright_Cyan,
            Bright_White
        };
        class TelnetSession{
            class Screen{
            public:
                class Cursor{
                    Screen& myScreen;
                    int X, Y;
                    bool wraps;
                    Color BGColor;
                    Color FGColor;

                public:
                    Cursor( Screen& screen );
                    Error ShouldWrap( bool shouldWrap = true );
                    Error Advance( int amount = 1 );
                    Error LineFeed( int amount = 1 );
                    Error CarriageReturn();
                    Error SetColor( Color foreground, Color background );
                    Error MoveTo( int x, int y );
                };
            private:
                int width, height;
                Cursor myCursor;
                bool supportsColor;
                bool supportsMovement;
                bool echos;
                CommStream stream;
            public:
                Screen( int Width, int Height );
                int Width();
                int Height();
            };


        public:

            TelnetSession();
            Error SendLine( std::string line );
            Error SendChar( const char c );
            bool HasLine();
            bool HasChar();
            std::string ReadLine();
            char ReadChar();
            std::string PeekLine();
            char PeekChar();


        };
    private:
        static void ConnectionHandler( CommStream &cs, Telnet *parent );
    public:
        Telnet();
        void Listen( int port, std::function<void(TelnetSession)> callback );

    };
}
#endif
