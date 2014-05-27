#ifndef MUD_GLOBAL_FILESYSTEM_HPP
#define MUD_GLOBAL_FILESYSTEM_HPP

#include <string>

namespace GlobalMUD{
    namespace Filesystem{
        bool FileExists(std::string path);
        size_t FileSize(std::string path);
    }
}

#endif
