#ifndef MUD_FILESYSTEM_MIME_HPP
#define MUD_FILESYSTEM_MIME_HPP

#include "libmudcommon/dllinfo.hpp"

#include <string>
namespace MUD{
    DLL_EXPORT std::string GetMimeFromExt( std::string ext );
}

#endif
