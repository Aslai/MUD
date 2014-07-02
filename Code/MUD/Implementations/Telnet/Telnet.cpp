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
    void Telnet::ConnectionHandler( CommStream cs, std::function<void(TelnetSession)> callback ){
        //Run the user's specified callback function
        TelnetSession ts( cs, *this );
        callback( ts );
    }

    Telnet::Telnet(){

    }

    Error Telnet::Listen( int port, std::function<void(TelnetSession)> callback ){
        //Start a CommStream daemon that will fire off new TelnetSession's on connect
        CommStream stream( CommStream::BINARY );
        return stream.Listen( port, std::bind( &Telnet::ConnectionHandler, this, std::placeholders::_1, callback ) );
    }

    Error Telnet::ReadTerms( std::string fname ){
        FILE* f = fopen( fname.c_str(), "r" );
        if( f == NULL )
            return Error::FileNotFound;
        Terminal temp;
        bool readingterm = false; //Indicates whether or not we're reading a terminal or parameter
        while( true ){
            char buffer[1000];

            fgets( buffer, 500, f );
            if( feof(f) )
                break;
            int strend = strlen( buffer );
            while(strend >= 0 && ( buffer[strend] == '\0' || isspace(buffer[strend]) ) ){
                buffer[strend--] = 0;
            }

            //If we're on a blank line...
            if( strlen( buffer ) == 0 ){
                //...And we were reading a definition...
                if( readingterm ){
                    //Then store it.
                    readingterm = false;
                    SupportedTerms[temp.Name] = temp;

                }
            }
            //Otherwise if we're not on a blank line...
            else{
                //And if we aren't reading a definition...
                if( !readingterm ){
                    //Then this is the name of the next terminal. Prepare to read parameters.
                    char* b = buffer;
                    while( *b != '\0' && isspace( *b ) )
                            b++;
                    strupr( b );
                    temp = Terminal();
                    temp.Name = b;
                    readingterm = true;

                }
                //But if we are...
                else{
                    char *iter = buffer;
                    while( *iter != ':' && *iter != '\0' ){
                        iter++;
                    }
                    if( *iter != '\0' ){
                        *iter = '\0';
                        char* Key = buffer;
                        char* Value = iter+1;

                        //Trim the whitespace off the start of Key and Value.
                        while( *Key != '\0' && isspace( *Key ) )
                            Key++;
                        while( *Value != '\0' && isspace( *Value ) )
                            Value++;

                        //If we actually have a parameter value...
                        if( strlen(Value) != 0 ){
                            strupr( Key );
                            strupr( Value );
                            bool affirmative = Value[0] == 'Y';

                            switch( HashString( Key ) ){
                                case HashString( "COLOR" ):         temp.Color = affirmative;           break;
                                case HashString( "WRAPS" ):         temp.Wraps = affirmative;           break;
                                case HashString( "ANSI-ESCAPE" ):   temp.ANSIEscape = affirmative;      break;
                                case HashString( "PREFERENCE" ):    temp.Preference = atol( Value );    break;
                                case HashString( "INHERIT" ): {
                                    std::string n = temp.Name;
                                    temp = SupportedTerms[Value];
                                    temp.Name = n;
                                }                                                                       break;
                                default: break;
                            }
                        }
                    }
                }
            }
        } //End of while

        //If we hit EOF before writing a terminal to the supported terminal map, write it.
        if( readingterm ){
            readingterm = false;
            SupportedTerms[temp.Name] = temp;
        }
        return Error::None;
    }
}

