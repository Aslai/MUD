//Be sure that Realm is defined in the case of a circular reference
#ifndef MUD_GAME_AREAS_REALM_DEFINED
#define MUD_GAME_AREAS_REALM_DEFINED
namespace GlobalMUD{
    namespace Game{
        class Realm;
    }
}
#endif // MUD_GAME_AREAS_REALM_DEFINED

#ifndef MUD_GAME_AREAS_REALM_HPP
#define MUD_GAME_AREAS_REALM_HPP

#include <vector>
#include <map>
#include <string>

#include "Game/Areas/Zone.hpp"
#include "Game/Areas/Room.hpp"
#include "Lua/Lua.hpp"

namespace GlobalMUD{
    namespace Game{
        class Realm{
            std::map<std::string, Zone> Zones;
            Lua::Value Properties;
            std::string id;
            Lua::Script RealmScript;
        public:
            static Realm& GetRealmByID( std::string ID );
        };
    }
}

#endif
