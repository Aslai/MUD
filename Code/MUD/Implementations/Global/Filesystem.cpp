
#include <string>
#include "Global/Filesystem.hpp"

#ifdef _WIN32
    #include <windows.h>
#else
    #include<sys/stat.h>
    #include <unistd.h>
#endif // _WIN32

namespace GlobalMUD{
    namespace Filesystem{
        #ifdef _WIN32
        bool FileExists(std::string path){
            DWORD attrib = GetFileAttributes(path.c_str());

            return (attrib != INVALID_FILE_ATTRIBUTES &&
            !(attrib & FILE_ATTRIBUTE_DIRECTORY));
        }

        size_t FileSize(std::string path){
            size_t ret;
            HANDLE f = CreateFile(path.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL,
                          OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);

            if( sizeof(ret) == sizeof(DWORD) ){
                size_t v = GetFileSize(f, NULL);
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
        bool FileExists(std::string path){
            return access(path.c_str(), F_OK);
        }

        size_t FileSize(std::string path){
            struct stat buffer;
            buffer.st_size = 0;
            int result = int stat(path.c_str(), &buffer);
            if( result == -1 )
                return 0;
            return buffer.st_size;
        }
        #endif
    }
}
