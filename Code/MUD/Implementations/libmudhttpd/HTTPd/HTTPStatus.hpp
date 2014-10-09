#ifndef MUD_HTTPD_STATUS_HPP
#define MUD_HTTPD_STATUS_HPP

#include "libmudcommon/dllinfo.hpp"

#include <string>
namespace MUD{
    DLL_EXPORT std::string GetMessageFromHTTPStatus( int status );
    DLL_EXPORT std::string GetDetailedMessageFromHTTPStatus( int status );
}

#endif
