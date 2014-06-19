#ifndef MUD_TELNET_TELNET_HPP
#define MUD_TELNET_TELNET_HPP


#include<map>
#include<string>
#include "Global/Error.hpp"
#include "CommStream/CommStream.hpp"
#include "Global/Thread.hpp"


namespace GlobalMUD{
    class Telnet{
    public:
        class TelnetSession{
        public:
            TelnetSession();

        };
        static void ConnectionHandler( CommStream &cs, void* data );
    public:
        Telnet();
        void Listen( int port );

    }
}
