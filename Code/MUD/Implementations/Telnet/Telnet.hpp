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
        enum class Commands {
            NONE,
            //Begin RFC 854 commands//
            SE = 240,   //Subnegotiation end				//0xF0
            NOP,        //No Op				//0xF1
            DataMark,   //Data mark				//0xF2
            BRK,        //Break				//0xF3
            IP,         //Interrupt process				//0xF4
            AO,         //Abort output				//0xF5
            AYT,        //Are you there?				//0xF6
            EC,         //Erase character				//0xF7
            EL,         //Erase line				//0xF8
            GA,         //Go ahead				//0xF9
            SB,         //Subnegotiation begin //0xFA
            WILL,		//0xFB
            WONT,		//0xFC
            DO,			//0xFD
            DONT,		//0xFE
            IAC,        //Command code / escape code 0xFF
            IS = 0,
            SEND = 1,

            //Begin RFC 1073 commands//
            NAWS = 31,  //Negotiate about window size
                        //(IAC SB NAWS [16 bit value] [16 bit value] IAC SE)
                        //Values in network order. Default is undefined.      //1F

            //Begin RFC 857 commands//
            ECHO = 1,   //Start/stop echoing. Default is no echo.

            //Begin RFC 858 commands//
            SUPPRESS_GO_AHEAD = 3,

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
        struct Terminal{
            public:
            std::string Name;
            bool Color;
            bool Wraps;
            bool ANSIEscape;
            long Preference;
        };
        class TelnetSession{
            friend Telnet;
            class Screen{
            public:
                class Cursor{
				friend Screen;
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
				friend Cursor;
            private:
                int width, height;
                Cursor myCursor;
                bool supportsColor;
                bool supportsMovement;
				TelnetSession &parent;
            public:
                Screen( int Width, int Height, TelnetSession &Parent );
                int Width();
                int Height();
                Error SetTerminal( std::string TerminalType );
				Error Resize( int w, int h );
            };
		private:
			Telnet &parent;
			bool echos;
			CommStream stream;
			void ReadStream();
			Error ParseCommand( char*& cmd, size_t len );
			std::string buffer;
			std::string bufferbacklog;
			int requestingTerminal;
			std::string bestTerminal;
			std::string lastTerminal;
			Mutex lock;



        public:
			Screen myScreen;

            TelnetSession( CommStream s, Telnet &Parent);

            Error SendLine( std::string line );
            Error SendChar( const char c );
            Error SendCommand( Telnet::Commands cmd1 = Telnet::Commands::NONE, Telnet::Commands cmd2 = Telnet::Commands::NONE );
            Error SendSubnegotiation( Telnet::Commands cmd, char* data = NULL, size_t len = 0 );
            Error SendSubnegotiation( Telnet::Commands cmd1, Telnet::Commands cmd2, char* data = NULL, size_t len = 0 );

            bool HasLine();
            bool HasChar();
            std::string ReadLine();
            char ReadChar();
            std::string PeekLine();
            char PeekChar();
			Error Disconnect( bool remoteFailure = false );


        };
        friend TelnetSession;
    private:
        void ConnectionHandler( CommStream cs, std::function<void(TelnetSession*)> callback );

    public:
        std::map<std::string, Terminal> SupportedTerms;
        Telnet();
        void Listen( int port, std::function<void(TelnetSession*)> callback );
        Error ReadTerms( std::string fname );

    };
}
#endif
