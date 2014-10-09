//Be sure that Realm is defined in the case of a circular reference
#ifndef MUD_GAME_AREAS_ROOM_DEFINED
#define MUD_GAME_AREAS_ROOM_DEFINED
namespace MUD{
    namespace Game{
        class Room;
    }
}
#endif // MUD_GAME_AREAS_REALM_DEFINED

#ifndef MUD_GAME_AREAS_ROOM_HPP
#define MUD_GAME_AREAS_ROOM_HPP

#include "Game/Areas/Zone.hpp"
#include "Game/Areas/Room.hpp"
#include "Lua/Lua.hpp"
#include "Game/Actors/Actor.hpp"
#include "Memory/RefCounter.hpp"

namespace MUD{
    namespace Game{
        class Room{
            Zone& MyZone;
            Lua::Value Properties;
            std::string id;
            std::vector<RefCounter<Actor>> Actors;
            Lua::Script RoomScript;

        public:
            enum class Exit{
                North = 0,
                East,
                South,
                West,
                Up,
                Down,
                NotAnExit
            };

            static Room& GetRoomByID( std::string RealmID, std::string ZoneID, std::string RoomID );
        };
    }
}

#endif
