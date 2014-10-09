//Be sure that Realm is defined in the case of a circular reference
#ifndef MUD_GAME_AREAS_ZONE_DEFINED
#define MUD_GAME_AREAS_ZONE_DEFINED
namespace MUD{
    namespace Game{
        class Zone;
    }
}
#endif // MUD_GAME_AREAS_REALM_DEFINED

#ifndef MUD_GAME_AREAS_ZONE_HPP
#define MUD_GAME_AREAS_ZONE_HPP

#include "Game/Areas/Zone.hpp"
#include "Game/Areas/Room.hpp"
#include "Lua/Lua.hpp"

namespace MUD{
    namespace Game{
        class Zone{
            Realm& MyRealm;
            std::map<std::string, Room> Rooms;
            Lua::Value Properties;
            std::string id;
            Lua::Script ZoneScript;
        public:

            static Zone& GetZoneByID( std::string RealmID, std::string ZoneID );

        };
    }
}

#endif
