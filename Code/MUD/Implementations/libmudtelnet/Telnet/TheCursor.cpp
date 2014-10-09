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

    DLL_EXPORT Telnet::TheScreen::TheCursor::TheCursor( TheScreen& screen ) : myScreen(screen) {
        X = Y = 0;
        wraps = false;
        isHidden = false;
    }

    DLL_EXPORT Error Telnet::TheScreen::TheCursor::ShouldWrap( bool shouldWrap ){
        wraps = shouldWrap;
        return Error::None;
    }

    DLL_EXPORT Error Telnet::TheScreen::TheCursor::Advance( int amount ){
        X += amount;
        if( X >= myScreen.Width() ){
            if( wraps ){
                Y += X / myScreen.Width();
                X %= myScreen.Width();
            }
            else{
                X = myScreen.Width() - 1;
            }
        }
        //CursorPosition is Row;Column format
        return myScreen.parent.SendANSICode( Telnet::ANSICodes::CursorPosition, Y + 1, X + 1 );
    }

    DLL_EXPORT Error Telnet::TheScreen::TheCursor::LineFeed( int amount ){
        Y += amount;
        if( Y >= myScreen.Height() ){
            Y = myScreen.Height() - 1;
        }
        return myScreen.parent.SendANSICode( Telnet::ANSICodes::CursorNextLine, amount );
    }

    DLL_EXPORT Error Telnet::TheScreen::TheCursor::CarriageReturn(){
        X = 0;
        return myScreen.parent.SendANSICode( Telnet::ANSICodes::CursorHorizontalAbsolute, 1 );
    }

    DLL_EXPORT Error Telnet::TheScreen::TheCursor::MoveTo( int x, int y ){
        //Screen coordinates are 1-based, so convert them to 0-based
        x--;
        y--;

        if( x < 0 || x >= myScreen.Width() || y < 0 || y >= myScreen.Height() ){
            return Error::OutOfBounds;
        }

        X = x;
        Y = y;

        //CursorPosition is Row;Column format
        return myScreen.parent.SendANSICode( Telnet::ANSICodes::CursorPosition, Y + 1, X + 1 );
    }

    DLL_EXPORT Error Telnet::TheScreen::TheCursor::Hide(){
        if( myScreen.parent.SendANSICode( Telnet::ANSICodes::CursorHide ) == Error::None )
            isHidden = true;
        else
            return Error::Unsupported;
        return Error::None;
    }

    DLL_EXPORT Error Telnet::TheScreen::TheCursor::Unhide(){
        if( myScreen.parent.SendANSICode( Telnet::ANSICodes::CursorShow ) == Error::None )
            isHidden = false;
        else
            return Error::Unsupported;
        return Error::None;
    }

    DLL_EXPORT bool Telnet::TheScreen::TheCursor::Hidden(){
        return isHidden;
    }

}

