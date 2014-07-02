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
        TelnetSession ts( cs, *this );
        callback( ts );
        //std::bind(callback, ts)();
    }

    Telnet::Telnet(){

    }

    Error Telnet::Listen( int port, std::function<void(TelnetSession)> callback ){
        CommStream stream( CommStream::BINARY );
        return stream.Listen( port, std::bind( &Telnet::ConnectionHandler, this, std::placeholders::_1, callback ) );
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

                }
                else{
                    char bufferA[500];
                    char bufferB[500];

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

                        if( strcmp( bA, "COLOR:" ) == 0 ){
                            temp.Color = affirmative;
                        }
                        if( strcmp( bA, "WRAPS:" ) == 0 ){
                            temp.Wraps = affirmative;
                        }
                        if( strcmp( bA, "ANSI-ESCAPE:" ) == 0 ){
                            temp.ANSIEscape = affirmative;
                        }
                        if( strcmp( bA, "PREFERENCE:" ) == 0 ){
                            temp.Preference = atol( bB );
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

