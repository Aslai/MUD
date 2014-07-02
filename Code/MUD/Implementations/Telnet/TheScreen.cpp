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
    Telnet::TheScreen::TheScreen( int Width, int Height, TelnetSessionInternal &Parent ) : parent(Parent), myCursor( *this ){
        width = Width;
        height = Height;

        supportsColor = parent.parent.SupportedTerms["DEFAULT"].Color;
        supportsEscapeCodes = parent.parent.SupportedTerms["DEFAULT"].ANSIEscape;
        myCursor.ShouldWrap( parent.parent.SupportedTerms["DEFAULT"].Wraps );

        BGColor = Telnet::Color::Default;
        FGColor = Telnet::Color::Default;

        TerminalType = "DEFAULT";

    }

    Telnet::TheScreen::TheCursor &Telnet::TheScreen::Cursor(){
        return myCursor;
    }

    Error Telnet::TheScreen::Clear(){
        return parent.SendANSICode( Telnet::ANSICodes::EraseDisplay, 2 );
    }

    int Telnet::TheScreen::Width(){
        return width;
    }

    int Telnet::TheScreen::Height(){
        return height;
    }

    Error Telnet::TheScreen::SetTerminal( std::string Type ){
        supportsColor = parent.parent.SupportedTerms[Type].Color;
        supportsEscapeCodes = parent.parent.SupportedTerms[Type].ANSIEscape;
        myCursor.ShouldWrap( parent.parent.SupportedTerms[Type].Wraps );

        TerminalType = Type;
        return Error::None;
    }

    std::string Telnet::TheScreen::GetTerminal( ){
        return TerminalType;
    }

    Error Telnet::TheScreen::Resize( int w, int h ){
        if( w <= 0 || h <= 0 )
            return Error::InvalidSize;
        width = w;
        height = h;
        //If the cursor was moved off the screen, bring it back in.
        if( myCursor.X >= w )
            myCursor.X = w - 1;
        if( myCursor.X < 0 )
            myCursor.X = 0;
        if( myCursor.Y >= h )
            myCursor.Y = h - 1;
        if( myCursor.Y < 0 )
            myCursor.Y = 0;

        parent.SendANSICode( Telnet::ANSICodes::CursorPosition, myCursor.Y, myCursor.X );

        return Error::None;
    }

    Error Telnet::TheScreen::SetColor( Color foreground, Color background ){

        if( BGColor != background ){
            BGColor = background;
            int idx = (int) background;
            int bright = false;
            //If the color index is a Bright_*** or Default, set the bright flag.
            if( idx >= 8){
                bright = true;
                idx -= 8;
            }
            //Since the bright flag is set if Default is chosen, let's check for default first.
            if( background == Telnet::Color::Default ){
                parent.SendANSICode( Telnet::ANSICodes::SelectGraphicRendition, (int)Telnet::SGRCodes::DefaultBackgroundColor );
            }
            else if( bright ){
                parent.SendANSICode( Telnet::ANSICodes::SelectGraphicRendition, (int)Telnet::SGRCodes::BackgroundColorBright + idx );
            }
            else {
                parent.SendANSICode( Telnet::ANSICodes::SelectGraphicRendition, (int)Telnet::SGRCodes::BackgroundColor + idx );
            }
        }

        if( FGColor != foreground ){
            FGColor = foreground;
            int idx = (int) foreground;
            int bright = false;
            //If the color index is a Bright_*** or Default, set the bright flag.
            if( idx >= 8){
                bright = true;
                idx -= 8;
            }
            //Since the bright flag is set if Default is chosen, let's check for default first.
            if( foreground == Telnet::Color::Default ){
                parent.SendANSICode( Telnet::ANSICodes::SelectGraphicRendition, (int)Telnet::SGRCodes::DefaultForegroundColor );
            }
            else if( bright ){
                parent.SendANSICode( Telnet::ANSICodes::SelectGraphicRendition, (int)Telnet::SGRCodes::ForegroundColorBright + idx );
            }
            else {
                parent.SendANSICode( Telnet::ANSICodes::SelectGraphicRendition, (int)Telnet::SGRCodes::ForegroundColor + idx );
            }
        }

        return Error::None;
    }
}

