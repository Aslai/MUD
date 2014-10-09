
#include <string>
#include "Filesystem/Filesystem.hpp"

#ifdef _WIN32
    #include <windows.h>
#else
    #include<sys/stat.h>
    #include <unistd.h>
#endif // _WIN32

namespace MUD{
    namespace Filesystem{
        #ifdef _WIN32
        DLL_EXPORT bool FileExists(std::string path){
            DWORD attrib = GetFileAttributes(path.c_str());

            return (attrib != INVALID_FILE_ATTRIBUTES &&
            !(attrib & FILE_ATTRIBUTE_DIRECTORY));
        }

        DLL_EXPORT size_t FileSize(std::string path){
            size_t ret;
            HANDLE f = CreateFile(path.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr,
                          OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, nullptr);

            if( sizeof(ret) == sizeof(DWORD) ){
                size_t v = GetFileSize(f, nullptr);
                CloseHandle(f);
                return v;
            }
            else{
                #ifdef _WIN64
                DWORD temp = 0;
                ret = GetFileSize(f, &temp);
                CloseHandle(f);
                size_t ret2 = temp;
                ret |= ret2 << sizeof(DWORD)*8;
                return ret;
                #endif
            }
            return 0;
        }
        #else
        DLL_EXPORT bool FileExists(std::string path){
            struct stat buffer;
            int result = stat(path.c_str(), &buffer);
            if( result == -1 )
                return false;
            return true;
        }

        DLL_EXPORT size_t FileSize(std::string path){
            struct stat buffer;
            buffer.st_size = 0;
            int result = stat(path.c_str(), &buffer);
            if( result == -1 )
                return 0;
            return buffer.st_size;
        }
        #endif
    }
}
