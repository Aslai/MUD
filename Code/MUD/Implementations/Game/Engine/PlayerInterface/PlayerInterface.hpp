#ifndef MUD_GAME_INTERFACE_INTERFACE_HPP
#define MUD_GAME_INTERFACE_INTERFACE_HPP

namespace GlobalMUD{
    namespace Game{
        namespace Interface{
            Actor* self;
            public:
            virtual ~Interface();

            virtual Error UpdateValue(std::string name, std::string value);
            virtual Error UpdateValue(std::string name, int value);
            virtual Error SendMessage(std::string message);

            Error PerformAction(std::string action);
            Error QueryValue(std::string name);

        }
    }
}

#endif
