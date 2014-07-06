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
#include "Memory/Stream.hpp"

namespace GlobalMUD{
    void Telnet::TelnetSessionInternal::ReadStream(){
        lock.Lock();
        Stream buff;

        do{
            size_t len = 1000;
            //Fill the stream with data from the CommStream
            switch( stream.Receive( (char*)buff.GetBuffer( len ), len ) ){
                case Error::NoData: len = 0; break;
                case Error::NotConnected: Disconnect( true ); len = 0; break;
                default: break;
            }
            buff.CommitBuffer( len );

            if( len == 0 )
                break;
            bool escaped = false;
            //Create a checkpoint where we can return to in order to read everything
            //preceding the next telnet instruction.
            auto start = buff.SaveCheckpoint();

            while( buff.HasChar() ){
                if( !escaped && (byte) buff.PeekChar() == (byte) Telnet::Commands::IAC ){

                    auto end = buff.SaveCheckpoint();
                    buff.LoadCheckpoint( start );
                    //Copy the information between the start and end checkpoints and dump it into the buffer.
                    buffer += BufferToString( (const char*) buff.GetData( end ), end-start );
                    start = buff.SaveCheckpoint();
                    buff.GetChar();

                    //If it turns out that the following character is another IAC, turn on escape mode.
                    if( buff.PeekByte() == (byte) Telnet::Commands::IAC ){
                        escaped = true;
                        start ++;
                        buff.GetChar();
                    }
                    else{
                        buff.LoadCheckpoint( start );
                        Error e = ParseCommand( buff );
                        //Move the start checkpoint to the next position in the data stream following
                        //the last telnet command.
                        start = buff.SaveCheckpoint();
                        if( e == Error::PartialData ){
                            break;
                        }
                    }

                }
                else{
                    escaped = false;
                    buff.GetChar();
                }
            }
            //If we've hit the end of the stream, dump the remaining data into buffer
            if( !buff.HasChar() ){
                auto end = buff.SaveCheckpoint();
                buff.LoadCheckpoint( start );

                std::string s = BufferToString( (char*)buff.GetData( end ), end-start );
                buffer += s;

                if( echos == true )
                    SendLine( s );
            }
        } while( stream.HasData() );
        lock.Unlock();
    }

    Error Telnet::TelnetSessionInternal::ParseCommand( Stream& cmd ){
        //Verify that this is actually a Telnet escape sequence
        if( cmd.PeekByte() != (byte)Telnet::Commands::IAC ){
            return Error::ParseFailure;
        }

        //Save a checkpoint to return to if a full command is unable to be read.
        auto start = cmd.SaveCheckpoint();

        cmd.GetByte();

        //If there is no byte to read, return to the starting checkpoint and report an error.
        if( !cmd.HasByte() ){
            cmd.LoadCheckpoint( start );
            return Error::PartialData;
        }
        int value = cmd.GetByte();

        Error e = Error::None;

        switch((Telnet::Commands)value){

            case Telnet::Commands::EC:      e = DoEC    ( cmd );    break;
            case Telnet::Commands::EL:      e = DoEL    ( cmd );    break;
            case Telnet::Commands::AYT:     e = DoAYT   ( cmd );    break;
            case Telnet::Commands::WILL:    e = DoWill  ( cmd );    break;
            case Telnet::Commands::WONT:    e = DoWont  ( cmd );    break;
            case Telnet::Commands::DO:      e = DoDo    ( cmd );    break;
            case Telnet::Commands::DONT:    e = DoDont  ( cmd );    break;
            case Telnet::Commands::SB:      e = DoSB    ( cmd );    break;

            case Telnet::Commands::NOP:
            case Telnet::Commands::IAC:
            case Telnet::Commands::IP:          //Not implemented
            case Telnet::Commands::AO:          //Not implemented
            case Telnet::Commands::DataMark:    //Not implemented
            case Telnet::Commands::BRK:         //Not implemented
            case Telnet::Commands::GA:          //Not implemented
            default:                                                break;

        }
        //If any of the subfunctions reported an error, return to the starting checkpoint.
        if( e != Error::None )
            cmd.LoadCheckpoint( start );
        return e;
    }

    Error Telnet::TelnetSessionInternal::DoEC( Stream& cmd ){
        //Erase the last character from the user's input stream.
        //Don't allow them to erase past a newline though.
        auto pos = buffer.find_last_of("\n");
        if( buffer.length() > 0 && (pos == std::string::npos || pos != buffer.length()-1) ){
            buffer.resize( buffer.length() - 1);
        }
        return Error::None;
    }

    Error Telnet::TelnetSessionInternal::DoEL( Stream& cmd ){
        //Erase the last line from the user's input stream.
        //Only the final line may be deleted.
        //Any line terminated with a '\n' cannot be deleted.
        auto pos = buffer.find_last_of("\n");
        if( buffer.length() > 0 && pos == std::string::npos ){
            buffer = "";
        }
        else if( buffer.length() > 0 ){
            buffer = buffer.substr( 0, pos + 1 );
        }
        return Error::None;
    }

    Error Telnet::TelnetSessionInternal::DoAYT( Stream& cmd ){
        //Send some printable response to the Are You There function.
        SendLine("Yes, I am here.");
        return Error::None;
    }

    Error Telnet::TelnetSessionInternal::DoDo( Stream& cmd ){
        if( !cmd.HasByte() )
            return Error::PartialData;
        int value = cmd.GetByte();

        //Respond positively to requests that the server supports

        switch((Telnet::Commands)value){
            case Telnet::Commands::NAWS:{
                SendCommand(Telnet::Commands::WILL, Telnet::Commands::NAWS );
            } break;
            case Telnet::Commands::ECHO:{
                SendCommand(Telnet::Commands::WILL, Telnet::Commands::ECHO );
                echos = true;
            } break;
            case Telnet::Commands::TERMINAL_TYPE:{
                SendCommand(Telnet::Commands::WILL, Telnet::Commands::TERMINAL_TYPE );

            } break;
            case Telnet::Commands::SUPPRESS_GO_AHEAD:{
                SendCommand(Telnet::Commands::WILL, Telnet::Commands::SUPPRESS_GO_AHEAD );
            } break;
            default:
                //If the server doesn't support it, respond negatively.
                SendCommand(Telnet::Commands::WONT, (Telnet::Commands)value );
                break;
        }
        return Error::None;
    }

    Error Telnet::TelnetSessionInternal::DoDont( Stream& cmd ){
        if( !cmd.HasByte() )
            return Error::PartialData;
        int value = cmd.GetByte();

        //Verify that the server won't do anything the client doesn't want to do.

        switch((Telnet::Commands) value){
            case Telnet::Commands::ECHO:{
                echos = false;
                SendCommand(Telnet::Commands::WONT, (Telnet::Commands)value );
            } break;
            case Telnet::Commands::NAWS:

            case Telnet::Commands::TERMINAL_TYPE:
            case Telnet::Commands::SUPPRESS_GO_AHEAD:
            default:
                SendCommand(Telnet::Commands::WONT, (Telnet::Commands)value );
                break;
        }
        return Error::None;
    }

    Error Telnet::TelnetSessionInternal::DoWill( Stream& cmd ){
        if( !cmd.HasByte() )
            return Error::PartialData;
        int value = cmd.GetByte();

        //Request the client do things that they offer to perform and that the server supports.

        switch((Telnet::Commands)value){
            case Telnet::Commands::NAWS:{
                SendCommand(Telnet::Commands::DO, Telnet::Commands::NAWS );
            } break;
            case Telnet::Commands::ECHO:{
                SendCommand(Telnet::Commands::DONT, Telnet::Commands::ECHO );
            } break;
            case Telnet::Commands::TERMINAL_TYPE:{
                SendCommand(Telnet::Commands::DO, Telnet::Commands::TERMINAL_TYPE );
                SendSubnegotiation( Telnet::Commands::TERMINAL_TYPE, Telnet::Commands::SEND );
            } break;
            case Telnet::Commands::SUPPRESS_GO_AHEAD:{
                SendCommand(Telnet::Commands::DO, Telnet::Commands::SUPPRESS_GO_AHEAD );
            } break;
            default:
                //Don't agree to do things the server doesn't know how to do.
                SendCommand(Telnet::Commands::DONT, (Telnet::Commands)value );
                break;
        }
        return Error::None;
    }
    Error Telnet::TelnetSessionInternal::DoWont( Stream& cmd ){
        if( !cmd.HasByte() )
            return Error::PartialData;
        int value = cmd.GetByte();

        //Since won't is a negative, no response is needed to an unsolicited won't request.

        switch((Telnet::Commands)value){
            case Telnet::Commands::NAWS:
            case Telnet::Commands::ECHO:
            case Telnet::Commands::TERMINAL_TYPE:
            case Telnet::Commands::SUPPRESS_GO_AHEAD:
            default:
                break;
        }
        return Error::None;
    }

    Error Telnet::TelnetSessionInternal::DoSB( Stream& cmd ){
        if( !cmd.HasByte() )
            return Error::PartialData;
        int value = cmd.GetByte();

        //Call related subnegotiation functions.

        Telnet::Commands thisCmd = (Telnet::Commands)value;
        switch( thisCmd ){
            case Telnet::Commands::NAWS:            return DoSubnegNAWS ( cmd );
            case Telnet::Commands::TERMINAL_TYPE:   return DoSubnegTTYPE( cmd );
            default: break;
        }
        return Error::None;
    }

    Error Telnet::TelnetSessionInternal::DoSubnegNAWS( Stream& cmd ){
        int state = 0;
        int value = 0;
        int w, h;
        w = h = 0;
        bool escaped = false;
        int len = 0;

        while( true ){
            if( !cmd.HasByte() )
                return Error::PartialData;
            value = cmd.GetByte();
            if( !escaped && value == (byte)Telnet::Commands::IAC ){
                //If the current value is IAC and not escaped, turn on escape mode.
                escaped = true;
                continue;
            }
            if( escaped && value == (byte)Telnet::Commands::SE ){
                //If this is the subnegotiation end, leave the function and report success.
                return Error::None;
            }
            if( len++ > 20 ){
                //If the subnegotiation isn't terminated within 20 characters, terminate anyways.
                //Escape sequences (IAC followed by any byte) count as one character.
                return Error::None;
            }

            switch( state++ ){
                //Read the high and low byte values for width and height.
                case 0: w += value << 8;        break;
                case 1: w += value;             break;
                case 2: h += value << 8;        break;
                case 3: h += value;
                        Screen.Resize( w, h );  break;
                default: break;
            }
            escaped = false;
        }
        return Error::None;
    }

    Error Telnet::TelnetSessionInternal::DoSubnegTTYPE( Stream& cmd ){
        int state = 0;
        int value = 0;
        std::string term = "";
        bool escaped = false;
        int len = 0;

        while( true ){
            if( !cmd.HasByte() )
                return Error::PartialData;
            value = cmd.GetByte();
            if( !escaped && value == (byte)Telnet::Commands::IAC ){
                //If an IAC is found without being in escape mode, turn escape mode on.
                escaped = true;
                continue;
            }
            if( escaped && value == (byte)Telnet::Commands::SE ){
                //Break the loop if the end of the subnegotiation is found.
                break;
            }
            if( len++ > 60 ){
                //If the subnegotiation isn't terminated within 60 characters, terminate anyways.
                //Escape sequences (IAC followed by any byte) count as one character.
                break;
            }

            switch( state++ ){
                case 0:
                    //If the IS was ommitted, tolerate it anyways.
                    //Add value to the terminal name if it's not an IS.
                    if( value != (unsigned char)Telnet::Commands::IS ){
                        term += (char) value;
                    } break;
                case 1:
                    if( value != (unsigned char)Telnet::Commands::IAC ){
                        term += (char) value;
                        state --;
                    } break;
                default: break;
            }
            escaped = false;
        }

        switch(requestingTerminal){
            case 0:{ //If we are looping through the list the first time
                if( term == lastTerminal ){
                    //If the last terminal was the same as this one, we've hit the end of the list.
                    //Move on to the next state.
                    requestingTerminal = 1;
                }
                lastTerminal = term;
                if( parent.SupportedTerms[StringToUpper(bestTerminal)].Preference <=
                        parent.SupportedTerms[StringToUpper(term)].Preference ){
                    bestTerminal = term;
                }
                //Request the client send the next terminal.
                SendSubnegotiation( Telnet::Commands::TERMINAL_TYPE, Telnet::Commands::SEND );
            } break;

            case 1:{ //Searching for the best terminal noticed during the first loop
                //If we find it, stop requesting more terminals.
                if( term == bestTerminal ){
                    PrintFormat( "Chosen terminal: %s\n", term );
                    requestingTerminal = 2;
                    Screen.SetTerminal( StringToUpper( term ) );
                }
                else
                    SendSubnegotiation( Telnet::Commands::TERMINAL_TYPE, Telnet::Commands::SEND );
            } break;

            default: break;
        }

        return Error::None;
    }


    Telnet::TelnetSessionInternal::TelnetSessionInternal( CommStream s, Telnet &Parent ) : parent(Parent), stream(s), lock(), Screen(80, 25, *this) {
        echos = false;
        requestingTerminal = 0;
        bestTerminal = "DEFAULT";
        lastTerminal = "";
        buffer = "";
        bufferbacklog = "";
        //Register the stream handling function with CommStream so it gets called automatically
        //every time new data ends up in the CommStream buffers.
        stream.RegisterCallback( std::bind(&Telnet::TelnetSessionInternal::ReadStream, this) );

        //Request that the client perform Terminal-Type
        SendCommand( Telnet::Commands::DO, Telnet::Commands::TERMINAL_TYPE );
    }

    Telnet::TelnetSessionInternal::~TelnetSessionInternal( ){
        stream.ClearCallbacks();
        Disconnect(false);
    }


    Error Telnet::TelnetSessionInternal::SendLine( std::string line ){
        return stream.Send( line );
    }

    Error Telnet::TelnetSessionInternal::SendChar( const char c ){
        std::string s = "";
        s += c;
        return stream.Send( s );
    }

    bool Telnet::TelnetSessionInternal::HasLine(){
        ScopedMutex m( lock );
        m.Lock();
        return buffer.find_first_of( "\n" ) != std::string::npos;
    }

    bool Telnet::TelnetSessionInternal::HasChar(){
        ScopedMutex m( lock );
        m.Lock();
        return buffer.length() > 0;
    }

    std::string Telnet::TelnetSessionInternal::ReadLine(){
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

    char Telnet::TelnetSessionInternal::ReadChar(){
        lock.Lock();
        char ret = '\0';
        if( buffer.length() > 0 ){
            ret = buffer[0];
            buffer.erase(0, 1);
        }
        lock.Unlock();
        return ret;
    }

    std::string Telnet::TelnetSessionInternal::PeekLine(){
        lock.Lock();
        auto pos = buffer.find_first_of( "\n" );
        std::string ret = "";
        if( pos != std::string::npos ){
            ret = buffer.substr( 0, pos + 1 );
        }
        lock.Unlock();
        return ret;
    }

    char Telnet::TelnetSessionInternal::PeekChar(){
        lock.Lock();
        char ret = '\0';
        if( buffer.length() > 0 ){
            ret = buffer[0];
        }
        lock.Unlock();
        return ret;
    }

    Error Telnet::TelnetSessionInternal::Disconnect( bool remoteFailure ){
        return stream.Disconnect(false);
    }

    Error Telnet::TelnetSessionInternal::SendCommand( Telnet::Commands cmd1, Telnet::Commands cmd2 ){
        std::string toSend = "";
        toSend += (unsigned char) Telnet::Commands::IAC;
        if( cmd1 != Telnet::Commands::NONE )
            toSend += (unsigned char) cmd1;
        if( cmd2 != Telnet::Commands::NONE )
            toSend += (unsigned char) cmd2;

        return SendLine( toSend );
    }

    Error Telnet::TelnetSessionInternal::SendSubnegotiation( Telnet::Commands cmd1, Telnet::Commands cmd2, char* data, size_t len ){
        std::string toSend = "";
        toSend += (unsigned char) Telnet::Commands::IAC;
        toSend += (unsigned char) Telnet::Commands::SB;
        if( cmd1 != Telnet::Commands::NONE )
            toSend += (unsigned char) cmd1;
        if( cmd2 != Telnet::Commands::NONE )
            toSend += (unsigned char) cmd2;
        if( len > 0 ){
            std::string toAdd = BufferToString( data, len );
            for( unsigned int i = 0; i < toAdd.length(); ++i ){
                if( (byte)toAdd[i] == (byte)Telnet::Commands::IAC ){
                    std::string toInsert = "";
                    toInsert += (unsigned char)Telnet::Commands::IAC;
                    toAdd.insert( i++, toInsert );
                }
            }
            toSend += toAdd;
        }

        toSend += (char) Telnet::Commands::IAC;
        toSend += (char) Telnet::Commands::SE;
        return SendLine( toSend );
    }

    Error Telnet::TelnetSessionInternal::SendSubnegotiation( Telnet::Commands cmd1, char* data, size_t len ){
        return SendSubnegotiation( cmd1, Telnet::Commands::NONE, data, len );
    }

    bool Telnet::TelnetSessionInternal::Connected(){
        return stream.Connected();
    }
}

