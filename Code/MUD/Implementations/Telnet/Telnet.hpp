#ifndef MUD_TELNET_TELNET_HPP
#define MUD_TELNET_TELNET_HPP


#include<map>
#include<string>
#include "Error/Error.hpp"
#include "CommStream/CommStream.hpp"
#include "Thread/Thread.hpp"
#include "Memory/Stream.hpp"

#include<functional>


namespace GlobalMUD{
    /////////////////////////////////////////////////
    ///
    /// Provides a robust Telnet server interface
    /// that handles all aspects of the Telnet
    /// protocol and permits the user of the object
    /// to use Telnet with no knowledge of the
    /// underlying implementation details.
    ///
    ///
    /// This particular implementation of the Telnet
    /// protocol only operates over TCP, as that is
    /// the prominent transport of choice these days.
    /// If one would be so inclined though, it would
    /// not be difficult to modify CommStream to
    /// support other transports, which would then
    /// permit the Telnet object to work over those
    /// new transport layers almost seamlessly.
    ///
    /////////////////////////////////////////////////
    class Telnet{
        class TelnetSessionInternal;
    public:

        /////////////////////////////////////////////////
        ///
        /// Provides named constants for various Telnet
        /// commands. These are provided for convenience
        /// and is by no means complete. Feel free to add
        /// commands as needed.
        ///
        /////////////////////////////////////////////////

        enum class Commands {
            NONE,                   ///<This placeholder is used for Telnet::TelnetSession::SendCommand()
                                    ///<if you do not wish to actually send a command in a given argument.\n
                                    ///<This is mainly used internally.
            //Begin RFC 854 commands//
            SE = 240,               ///<_0xF0_ - Subnegotiation end __RFC 854__
            NOP,                    ///<_0xF1_ - No Operation __RFC 854__
            DataMark,               ///<_0xF2_ - Data mark __RFC 854__
            BRK,                    ///<_0xF3_ - Break __RFC 854__
            IP,                     ///<_0xF4_ - Interrupt process __RFC 854__
            AO,                     ///<_0xF5_ - Abort output __RFC 854__
            AYT,                    ///<_0xF6_ - Are you there? __RFC 854__
            EC,                     ///<_0xF7_ - Erase character __RFC 854__
            EL,                     ///<_0xF8_ - Erase line __RFC 854__
            GA,                     ///<_0xF9_ - Go ahead __RFC 854__
            SB,                     ///<_0xFA_ - Subnegotiation begin __RFC 854__

            WILL,		            ///<_0xFB_ - Will command __RFC 854__\n
                                    ///<Send this to say you are willing to perform a specified option.\n
                                    ///<This is the appropriate positive response to a Do command.

            WONT,		            ///<_0xFC_ - Won't command __RFC 854__\n
                                    ///<Send this to say you are not willing to perform a specified option.\n
                                    ///<This is the appropriate negative response to a Do command.

            DO,			            ///<_0xFD_ - Do command __RFC 854__\n
                                    ///<Send this to request the other side perform a specified option.\n
                                    ///<This is the appropriate positive response to a Will command.

            DONT,		            ///<_0xFE_ - Don't command __RFC 854__\n
                                    ///<Send this to request the other side to not perform a specified option.\n
                                    ///<This is the appropriate negative response to a Will command.

            IAC,                    ///<_0xFF_ - Command code / escape code __RFC 854__\n
                                    ///<This denotes the start of an escape sequence.

            IS = 0,                 ///<_0x00_ - Is directive __RFC 854__\n
            SEND = 1,               ///<_0x01_ - Send directive __RFC 854__

            //Begin RFC 1073 commands//
            NAWS = 31,              ///<_0x1F_ - Negotiate about window size option __RFC 1073__\n
                                    ///<Format: __IAC__ __SB__ __NAWS__ [ _16 bit value_ ] [ _16 bit value_ ] __IAC__ __SE__\n
                                    ///<Values are in network order.\n
                                    ///<Default is __80 x 25__

            //Begin RFC 857 commands//
            ECHO = 1,               ///<_0x01_ - Start/stop echoing option __RFC 875__\n
                                    ///<Default is to not echo.

            //Begin RFC 858 commands//
            SUPPRESS_GO_AHEAD = 3,  ///<_0x03_ - Suppress-Go-Ahead option __RFC 858__\n
                                    ///<Puts the terminal into character mode if enabled.\n
                                    ///<Default is to not suppress go ahead.

            //Begin RFC 1091 commands//
            TERMINAL_TYPE = 24      ///<_0x18_ - Terminal-Type option __RFC 1091__\n
                                    ///<Selects a terminal type so the server knows the capabilities of the client.\n
                                    ///<Look up __RFC 1091__ for detailed usage info.\n
                                    ///<Default is __DEFAULT__.

        };

        /////////////////////////////////////////////////
        ///
        /// Provides named constants for the different
        /// colors available to most terminals.\n
        /// XTERM 256 colors were not deemed necessary,
        /// however if they are needed, XTERM 256 colors
        /// would be trivial to implement.
        ///
        /////////////////////////////////////////////////

        enum class Color{
            Black = 0,      ///<Add this to a relevant SGR color code to obtain the SGR code for black
            Red,            ///<Add this to a relevant SGR color code to obtain the SGR code for red
            Green,          ///<Add this to a relevant SGR color code to obtain the SGR code for green
            Yellow,         ///<Add this to a relevant SGR color code to obtain the SGR code for yellow
            Blue,           ///<Add this to a relevant SGR color code to obtain the SGR code for blue
            Magenta,        ///<Add this to a relevant SGR color code to obtain the SGR code for magenta
            Cyan,           ///<Add this to a relevant SGR color code to obtain the SGR code for cyan
            White,          ///<Add this to a relevant SGR color code to obtain the SGR code for white
            Bright_Black,   ///<For API convenience only.\n
                            ///<__Do not use this in an SGR color code__.
            Bright_Red,     ///<For API convenience only.\n
                            ///<__Do not use this in an SGR color code__.
            Bright_Green,   ///<For API convenience only.\n
                            ///<__Do not use this in an SGR color code__.
            Bright_Yellow,  ///<For API convenience only.\n
                            ///<__Do not use this in an SGR color code__.
            Bright_Blue,    ///<For API convenience only.\n
                            ///<__Do not use this in an SGR color code__.
            Bright_Magenta, ///<For API convenience only.\n
                            ///<__Do not use this in an SGR color code__.
            Bright_Cyan,    ///<For API convenience only.\n
                            ///<__Do not use this in an SGR color code__.
            Bright_White,   ///<For API convenience only.\n
                            ///<__Do not use this in an SGR color code__.
            Default = 9     ///<For API convenience only.\n
                            ///<__Do not use this in an SGR color code__.\n
                            ///<Selects the default color for the terminal.\n
                            ///<This is implementation defined.
        };

        /////////////////////////////////////////////////
        ///
        /// Provides named constants for various ANSI
        /// escape sequences.\n
        /// To use one, pass it into
        /// Telnet::TelnetSession::SendANSICode().
        ///
        /////////////////////////////////////////////////

        enum class ANSICodes{
            CursorUp,                       ///<Format: __CursorUp__ [ _n_ ]\n
                                            ///<This moves the cursor upwards by _n_ spaces.\n
                                            ///<If the cursor is already at the top of the screen, this command will have no effect.

            CursorDown,                     ///<Format: __CursorDown__ [ _n_ ]\n
                                            ///<This moves the cursor downwards by _n_ spaces.\n
                                            ///<If the cursor is already at the bottom of the screen, this command will have no effect.

            CursorForward,                  ///<Format: __CursorForward__ [ _n_ ]\n
                                            ///<This moves the cursor forwards by _n_ spaces.\n
                                            ///<If the cursor is already at the far right of the screen, this command will have no effect.

            CursorBack,                     ///<Format: __CursorBack__ [ _n_ ]\n
                                            ///<This moves the cursor backwards by _n_ spaces.\n
                                            ///<If the cursor is already at the far left of the screen, this command will have no effect.

            CursorNextLine,                 ///<Format: __CursorNextLine__ [ _n_ ]\n
                                            ///<This moves the cursor to the _n_-th next line

            CursorPreviousLine,             ///<Format: __CursorForward__ [ _n_ ]\n
                                            ///<This moves the cursor to the _n_-th previous line.

            CursorHorizontalAbsolute,       ///<Format: __CursorHorizontalAbsolute__ [ _n_ ]\n
                                            ///<This moves the cursor to the _n_-th column of the terminal.

            CursorPosition,                 ///<Format: __CursorPosition__ [ _r_ [ _c_ ] ]\n
                                            ///<This moves the cursor to the _r_-th row and _c_-th column.\n
                                            ///<If a value is unspecified, the value __1__ is assumed.\n
                                            ///<This is the equivalent of __HorizontalAndVerticalPosition__.

            EraseDisplay,                   ///<Format: __EraseDisplay__ [ _n_ ]\n
                                            ///<This clears the screen with different behavior based on the value of _n_\n
                                            ///<If _n_ is 0 or unspecified, clear from the cursor to the end of the terminal.\n
                                            ///<If _n_ is 1, clear from the cursor to the beginning of the terminal.\n
                                            ///<If _n_ is 2, clear the entire terminal.

            EraseInLine,                    ///<Format: __EraseLine__ [ _n_ ]\n
                                            ///<This erases the current line based on what _n_ is.
                                            ///<If _n_ is 0 or unspecified, clear from the cursor to the end of the line.\n
                                            ///<If _n_ is 1, clear from the cursor to the beginning of the line.\n
                                            ///<If _n_ is 2, clear the entire line.

            ScrollUp,                       ///<Format: __ScrollUp__ [ _n_ ]\n
                                            ///<This scrolls the terminal up _n_ lines by inserting _n_ lines at the bottom.

            ScrollDown,                     ///<Format: __ScrollDown__ [ _n_ ]\n
                                            ///<This scrolls the terminal down _n_ lines by inserting _n_ lines at the top

            HorizontalAndVerticalPosition,  ///<Format: __HorizontalAndVerticalPosition__ [ _r_ [ _c_ ] ]\n
                                            ///<This moves the cursor to the _r_-th row and _c_-th column.\n
                                            ///<If a value is unspecified, the value __1__ is assumed.\n
                                            ///<This is the equivalent of __CursorPosition__.

            SelectGraphicRendition,         ///<Format: __SelectGraphicRendition__ [ _n_ ]\n
                                            ///<This changes the current graphics setting of the terminal.\n
                                            ///<See GlobalMUD::Telnet::SGRCodes for possible values and explanations for their behavior.

            DeviceStatusReport,             ///<Format: __DeviceStatusReport__\n
                                            ///<This reports the cursor position to the application as though it was typed at the keyboard.

            CursorSavePosition,             ///<Format: __CursorSavePosition__\n
                                            ///<This Saves the cursor position.

            CursorRestorePosition,          ///<Format: __CursorRestorePosition__\n
                                            ///<This Restores the cursor position.

            CursorHide,                     ///<Format: __CursorHide__\n
                                            ///<This hides the cursor.

            CursorShow                      ///<Format: __CursorShow__\n
                                            ///<This shows the cursor.

        };

        /////////////////////////////////////////////////
        ///
        /// Provides named constants for various
        /// Select Graphic Rendition operations.\n
        /// To use one, pass it as an argument to
        /// Telnet::TelnetSession::SendANSICode()
        /// when the chosen command is
        /// Telnet::ANSICodes::SelectGraphicRendition
        ///
        /////////////////////////////////////////////////

        enum class SGRCodes{
            Reset = 0,                  ///<Resets all formatting
            Bold,                       ///<Makes text appear more bold
            Underline = 4,              ///<Underlines the text
            Blinking,                   ///<Makes text blink
            Negative = 7,               ///<Swaps the background and foreground colors
            DefaultFont = 10,           ///<Set the font to the default font
            Font1,                      ///<Set the font to font #1
            Font2,                      ///<Set the font to font #2
            Font3,                      ///<Set the font to font #3
            Font4,                      ///<Set the font to font #4
            Font5,                      ///<Set the font to font #5
            Font6,                      ///<Set the font to font #6
            Font7,                      ///<Set the font to font #7
            Font8,                      ///<Set the font to font #8
            Font9,                      ///<Set the font to font #9
            NormalColor = 22,           ///<Makes text not bold
            NotUnderlined = 24,         ///<Makes text not underlined
            NotBlinking,                ///<Makes text not blinking
            Positive = 27,              ///<Makes background and foreground colors revert to normal
            ForegroundColor = 30,       ///<Change the foreground color\n
                                        ///<Add a Telnet::Color to this value to get the desired color

            DefaultForegroundColor = 39,///<Change the foreground color to the default foreground color

            BackgroundColor = 40,       ///<Change the background color\n
                                        ///<Add a Telnet::Color to this value to get the desired color

            DefaultBackgroundColor = 49,///<Change the background color to the default background color
            Framed = 51,                ///<Frames the text
            Encircled,                  ///<Encircles the text
            Overlined,                  ///<Overlines the text
            NotEncircled,               ///<Makes text not encircled
            NotOverlined,               ///<Makes text not overlined
            ForegroundColorBright = 90, ///<Change the foreground color to a bright variant of a color\n
                                        ///<Add a Telnet::Color to this value to get the desired color

            BackgroundColorBright = 100 ///<Change the background color to a bright variant of a color\n
                                        ///<Add a Telnet::Color to this value to get the desired color

        };

        class TelnetSession;
        class TheScreen;

        /////////////////////////////////////////////////
        ///
        /// This structure is intended to hold values
        /// related to the capabilities of various
        /// terminals and terminal emulators.
        ///
        /////////////////////////////////////////////////

        struct Terminal{
            public:
            std::string Name;   ///<The name of the terminal type
            bool Color;         ///<Does this terminal type at least support the ANSI SGR code?
            bool Wraps;         ///<Does this terminal type automatically wrap lines?
            bool ANSIEscape;    ///<Does this terminal type support the full range of ANSI escape codes?
            long Preference;    ///<Higher preference means the server will prioritize establishing this
                                ///<terminal type over other types with a lower preference.\n
                                ///<Generally, the more capable terminal types should have higher preference.
        };

        /////////////////////////////////////////////////
        ///
        /// TheScreen is intended to provide an intuitive
        /// means of querying terminal information and
        /// changing terminal graphics settings.
        ///
        /////////////////////////////////////////////////

        class TheScreen{
        friend TelnetSession;
        friend TelnetSessionInternal;

        public:

            /////////////////////////////////////////////////
            ///
            /// TheCursor is intended to provide an intuitive
            /// means of querying information about the
            /// terminal's cursor, as well as provide a simple
            /// method of moving the cursor from user code.
            ///
            /////////////////////////////////////////////////

            class TheCursor{
            friend TheScreen;
                TheScreen& myScreen;
                int X, Y;
                bool isHidden;
                bool wraps;
            public:
                TheCursor( TheScreen& screen );
                Error ShouldWrap( bool shouldWrap = true );
                Error Advance( int amount = 1 );
                Error LineFeed( int amount = 1 );
                Error CarriageReturn();
                Error MoveTo( int x, int y );
                Error Hide();
                Error Unhide();
                bool Hidden();
            };
            friend TheCursor;
        private:
            int width, height;
            bool supportsColor;
            bool supportsEscapeCodes;
            TelnetSessionInternal &parent;
            Color BGColor;
            Color FGColor;
            std::string TerminalType;
            TheCursor myCursor;

        public:
            TheCursor &Cursor();
            TheScreen( int Width, int Height, TelnetSessionInternal &Parent );
            int Width();
            int Height();
            Error Clear();
            Error SetTerminal( std::string TerminalType );
            std::string GetTerminal();
            Error Resize( int w, int h );
            Error SetColor( Color foreground, Color background );
        };

    private:
        class TelnetSessionInternal{
            friend Telnet;
        public:
            friend TheScreen;
		private:
			Telnet &parent;
			bool echos;
			CommStream stream;
			void ReadStream();
			Error ParseCommand( Stream& cmd );
			Error DoDo( Stream& cmd );
			Error DoDont( Stream& cmd );
			Error DoWill( Stream& cmd );
			Error DoWont( Stream& cmd );
			Error DoSB( Stream& cmd );
			Error DoEC( Stream& cmd );
			Error DoEL( Stream& cmd );
			Error DoAYT( Stream& cmd );

			Error DoSubnegNAWS( Stream& cmd );
			Error DoSubnegTTYPE( Stream& cmd );


			std::string buffer;
			std::string bufferbacklog;
			int requestingTerminal;
			std::string bestTerminal;
			std::string lastTerminal;
			Mutex lock;



        public:
			TheScreen Screen;

            TelnetSessionInternal( CommStream s, Telnet &Parent);
            ~TelnetSessionInternal();

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
                stream.Flush();
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
                        char buffer[20*10];
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
        public:

        /////////////////////////////////////////////////
        ///
        /// TelnetSession is intended to provide an
        /// abstracted interface between user code and
        /// the underlying Telnet protocol, along with
        /// utilities that permit user code the ability
        /// to safely try to use ANSI terminal manipulation
        /// codes without risking sending garbage to a
        /// terminal that doesn't support them.
        ///
        /////////////////////////////////////////////////

        class TelnetSession{
			RefCounter<TelnetSessionInternal> internal;
        public:
			TheScreen& Screen();
            TelnetSession( CommStream s, Telnet &Parent);
            ~TelnetSession();
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
                return internal->SendANSICode( code, args... );
            }
        };
        friend TelnetSession;
    private:
        void ConnectionHandler( CommStream cs, std::function<void(TelnetSession)> callback );
        std::map<std::string, Terminal> SupportedTerms;
        CommStream stream;
    public:


/*!****************************************
\brief __Construct a Telnet object__

This function constructs the Telnet object for later use.

__Example__

~~~~~~~~~~~~~~~{.cpp}
#include "Telnet/Telnet.hpp"

int main( int argc, char *argv[] ){
    GlobalMUD::Telnet tnet;
    return 0;
}
~~~~~~~~~~~~~~~

****************************************/
        Telnet();

/*!****************************************
\brief __Start listening for connections__

This function opens the specified port and waits for
potential users to connect. Upon a successful connection,
the provided callback will be called.

\param port     The port that the telnet server shall listen on
\param callback A function that will be called every time
                a client connects to the server.

\return
    __Error::ConnectionFailure__: The connection has failed for some reason.\n
    __Error::None__: The server has shut down gracefully.

__Example__

~~~~~~~~~~~~~~~{.cpp}
#include "Telnet/Telnet.hpp"
#include <iostream> //for std::cout
#include <functional> //for std::bind()

void callback( GlobalMUD::Telnet::TelnetSession session ){
    std::cout << "Made it into the callback!";
}

int main( int argc, char *argv[] ){
    GlobalMUD::Telnet tnet;
    tnet.Listen( 23, std::bind(callback) );
    return 0;
}
~~~~~~~~~~~~~~~

****************************************/
        Error Listen( int port, std::function<void(TelnetSession)> callback );

/*!****************************************
\brief __Read a list of terminal capabilities from a file__

This function opens the specified file for reading and proceeds
to iterate over all of the lines in the file. The format of the
file is expected to be provided in a very specific fashion.

__Expected File Format__

- The file should be written entirely in caps. While efforts have
  been made to correct for this automatically, consistency is nice.
- When adding a terminal definition, the name of the terminal shall
  be on its own line, followed by a list of parameters.
- Parameters are placed on their own line, and are followed by a
  colon, a space, and then a value. For binary values, it shall be
  either __YES__ or __NO__. Numeric values can be any whole number
  between -(2*31) and (2^31)-1. String values may be any
  valid ASCII string.
- A terminal definition is terminated by either a blank line or EOF.


__Available Parameters__

Name        |Type    |Description
------------|--------|------------------------------------------------------------
COLOR       |Binary  |Does this terminal support ANSI SGR escape sequences?
WRAPS       |Binary  |Does this terminal wrap by default?
ANSI-ESCAPE |Binary  |Does this terminal support all ANSI escape sequences?
PREFERENCE  |Integer |What is the server's preference for choosing this terminal? (Higher values means that it's more likely to be chosen)
INHERIT     |String  |Copy the parameters from another terminal. This will overwrite any values stored so far. You may also only inherit from a terminal type that occurs prior to using the INHERIT parameter.


__Example %Terminal Specification File__

    DEFAULT
    COLOR: NO
    WRAPS: NO
    ANSI-ESCAPE: NO
    PREFERENCE: 0

    DUMB
    INHERIT DEFAULT

    ANSI
    COLOR: YES
    WRAPS: YES
    ANSI-ESCAPE: YES
    PREFERENCE: 1

    VT100
    INHERIT: ANSI
    PREFERENCE: 2

\param fname The file that contains a list of terminals and
             their capabilities.

\return
    __Error::FileNotFound__: The specified file could not be found.\n
    __Error::None__: The operation completed successfully.

__Example__

~~~~~~~~~~~~~~~{.cpp}
#include "Telnet/Telnet.hpp"
#include <iostream> //for std::cout
#include <functional> //for std::bind()

void callback( GlobalMUD::Telnet::TelnetSession session ){
    std::cout << session.Screen().GetTerminal();
}

int main( int argc, char *argv[] ){
    GlobalMUD::Telnet tnet;
    if( tnet.ReadTerms("terminals.txt") != Error::None )
        return 1;
    tnet.Listen( 23, std::bind(callback) );
    return 0;
}
~~~~~~~~~~~~~~~

****************************************/
        Error ReadTerms( std::string fname );

        Error Disconnect();

    };
}
#endif
