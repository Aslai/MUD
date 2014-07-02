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

    Telnet::TheScreen::TheCursor::TheCursor( TheScreen& screen ) : myScreen(screen) {
        X = Y = 0;
        wraps = false;
    }

    Error Telnet::TheScreen::TheCursor::ShouldWrap( bool shouldWrap ){
        wraps = shouldWrap;
        return Error::None;
    }

    Error Telnet::TheScreen::TheCursor::Advance( int amount ){
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

    Error Telnet::TheScreen::TheCursor::LineFeed( int amount ){
        if( Y < myScreen.Height() - 1 )
            Y++;
        return Error::None;
    }

    Error Telnet::TheScreen::TheCursor::CarriageReturn(){
        X = 0;
        return Error::None;
    }

    Error Telnet::TheScreen::TheCursor::MoveTo( int x, int y ){
        if( X < 0 || X >= myScreen.Width() || Y < 0 || Y >= myScreen.Height() ){
            return Error::OutOfBounds;
        }
        X = x;
        Y = y;
        return Error::None;
    }
}

