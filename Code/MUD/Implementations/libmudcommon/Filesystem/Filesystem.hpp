#ifndef MUD_FILESYSTEM_FILESYSTEM_HPP
#define MUD_FILESYSTEM_FILESYSTEM_HPP

#include "libmudcommon/dllinfo.hpp"

#include <string>

namespace MUD{
    namespace Filesystem{
        DLL_EXPORT bool FileExists(std::string path);
        DLL_EXPORT size_t FileSize(std::string path);
    }
}

#endif
