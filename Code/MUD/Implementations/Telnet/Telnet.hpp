#ifndef MUD_TELNET_TELNET_HPP
#define MUD_TELNET_TELNET_HPP


#include<map>
#include<string>
#include "Error/Error.hpp"
#include "CommStream/CommStream.hpp"
#include "Thread/Thread.hpp"

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
            TERMINAL_TYPE = 24 //18

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
            Bright_White,
            Default = 9
        };
        enum class ANSICodes{
            CursorUp,
            CursorDown,
            CursorForward,
            CursorBack,
            CursorNextLine,
            CursorPreviousLine,
            CursorHorizontalAbsolute,
            CursorPosition,
            EraseDisplay,
            EraseInLine,
            ScrollUp,
            ScrollDown,
            HorizontalAndVerticalPosition,
            SelectGraphicRendition,
            DeviceStatusReport,
            CursorSavePosition,
            CursorRestorePosition,
            CursorHide,
            CursorShow
        };
        enum class SGRCodes{
            Reset = 0,
            Bold,
            Underline = 4,
            Blinking,
            Negative = 7,
            DefaultFont = 10,
            Font1,
            Font2,
            Font3,
            Font4,
            Font5,
            Font6,
            Font7,
            Font8,
            Font9,
            NormalColor = 22,
            NotUnderlined = 24,
            NotBlinking,
            Positive = 27,
            ForegroundColor = 30, //ForegroundColor + Color
            DefaultForegroundColor = 39,
            BackgroundColor = 40, //BackgroundColor + Color
            DefaultBackgroundColor = 49,
            Framed = 51,
            Encircled,
            Overlined,
            NotEncircled,
            NotOverlined,
            ForegroundColorBright = 90,
            BackgroundColorBright = 100
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
            class TheScreen{
            friend TelnetSession;
            public:
                class Cursor{
				friend TheScreen;
                    TheScreen& myScreen;
                    int X, Y;
                    bool wraps;
                public:
                    Cursor( TheScreen& screen );
                    Error ShouldWrap( bool shouldWrap = true );
                    Error Advance( int amount = 1 );
                    Error LineFeed( int amount = 1 );
                    Error CarriageReturn();
                    Error MoveTo( int x, int y );
                };
				friend Cursor;
            private:
                int width, height;
                bool supportsColor;
                bool supportsEscapeCodes;
				TelnetSession &parent;
                Color BGColor;
                Color FGColor;

            public:
                Cursor myCursor;
                TheScreen( int Width, int Height, TelnetSession &Parent );
                int Width();
                int Height();
                Error SetTerminal( std::string TerminalType );
				Error Resize( int w, int h );
                Error SetColor( Color foreground, Color background );
            };
            friend TheScreen;
		private:
			Telnet &parent;
			bool echos;
			CommStream stream;
			void ReadStream();
			Error ParseCommand( unsigned char*& cmd, size_t len );
			std::string buffer;
			std::string bufferbacklog;
			int requestingTerminal;
			std::string bestTerminal;
			std::string lastTerminal;
			Mutex lock;



        public:
			TheScreen Screen;

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
			bool Connected();

            template<class... Args>
            Error SendANSICode( ANSICodes code, Args... args ){
                if( !Screen.supportsColor && code == Telnet::ANSICodes::SelectGraphicRendition ){
                    return Error::Unsupported;
                }
                if( !Screen.supportsEscapeCodes ){
                    return Error::Unsupported;
                }

                struct{
                    std::string v;
                    int operator()(int i){
                        if( v != "" )
                            v = ";" + v;
                        char buffer[20];
                        snprintf(buffer, 20, "%d", i );
                        v = buffer + v;
                        return 0;
                    }
                    int operator()(SGRCodes i){
                        return operator()((int)i);
                    }
                    int operator()(ANSICodes i){
                        return operator()((int)i);
                    }
                }combine;
                struct{void operator()(...){}}f;

                f(combine(args)...);
                std::string CSI = "\x1B[";
                combine.v = CSI + combine.v;
                switch( code ){
                    case Telnet::ANSICodes::CursorUp:                       combine.v += "A";           break;
                    case Telnet::ANSICodes::CursorDown:                     combine.v += "B";           break;
                    case Telnet::ANSICodes::CursorForward:                  combine.v += "C";           break;
                    case Telnet::ANSICodes::CursorBack:                     combine.v += "D";           break;
                    case Telnet::ANSICodes::CursorNextLine:                 combine.v += "E";           break;
                    case Telnet::ANSICodes::CursorPreviousLine:             combine.v += "F";           break;
                    case Telnet::ANSICodes::CursorHorizontalAbsolute:       combine.v += "G";           break;
                    case Telnet::ANSICodes::CursorPosition:                 combine.v += "H";           break;
                    case Telnet::ANSICodes::EraseDisplay:                   combine.v += "J";           break;
                    case Telnet::ANSICodes::EraseInLine:                    combine.v += "K";           break;
                    case Telnet::ANSICodes::ScrollUp:                       combine.v += "S";           break;
                    case Telnet::ANSICodes::ScrollDown:                     combine.v += "T";           break;
                    case Telnet::ANSICodes::HorizontalAndVerticalPosition:  combine.v += "f";           break;
                    case Telnet::ANSICodes::SelectGraphicRendition:         combine.v += "m";           break;
                    case Telnet::ANSICodes::DeviceStatusReport:             combine.v = CSI + "6n";     break;
                    case Telnet::ANSICodes::CursorSavePosition:             combine.v = CSI + "s";      break;
                    case Telnet::ANSICodes::CursorRestorePosition:          combine.v = CSI + "u";      break;
                    case Telnet::ANSICodes::CursorHide:                     combine.v = CSI + "?25l";   break;
                    case Telnet::ANSICodes::CursorShow:                     combine.v = CSI + "?25h";   break;
                    default: break;

                }
                return SendLine( combine.v );
            }


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
