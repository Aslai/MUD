#ifndef MUD_HTTPD_HTTPD_HPP
#define MUD_HTTPD_HTTPD_HPP

#include "libmudcommon/dllinfo.hpp"

#include<map>
#include<string>
#include "libmudcommon/Error/Error.hpp"
#include "libmudcommon/CommStream/CommStream.hpp"
#include "libmudcommon/Thread/Thread.hpp"
#include "libmudcommon/Memory/Stream.hpp"

namespace MUD{
    class HTTPd{
    public:
        struct HTTPResponse{
            int status;
            std::map<std::string,std::string> headers;
            std::map<std::string,std::string> gets;
            std::map<std::string,std::string> posts;
            std::string request;
            std::string filepath;
            std::vector<char> content;
            DLL_EXPORT void SetContent(std::string value);
            DLL_EXPORT HTTPResponse& operator+=(HTTPResponse other);
        };
        struct MountPoint{
            enum Types{
                BAD = 0,
                FOLDER,
                FILE,
                FUNCTION
            } Type;
            std::string Path;
            std::string MountPath;
            std::function<HTTPResponse(HTTPResponse response, HTTPd& parent)> Func;
        };
        struct FileHandler{
            std::string Extension;
            std::function<HTTPResponse(HTTPResponse response, HTTPd& parent, std::string localpath)> Func;
            bool Valid;
        };

    private:
        std::map<std::string, MountPoint> MountPoints;
        std::map<std::string, FileHandler> FileHandlers;
        std::string Address;
        std::string DefaultIndex;
        int Port;
        CommStream *stream;
        Thread *MyThread;
        static void ConnectionHandler(CommStream stream, void* argument );
        static void ConnectionHandlerThread(CommStream stream, void* parent);
        static std::string ParseRequest(Stream &data, HTTPResponse &r, std::string defaultIndex);

    public:
        DLL_EXPORT Error MountFolder(std::string mountpath, std::string folderpath);
        DLL_EXPORT Error MountDirectory(std::string mountpath, std::string folderpath);
        DLL_EXPORT Error MountFile(std::string mountpath, std::string filepath);
        DLL_EXPORT Error MountFunction(std::string mountpath, std::function<HTTPResponse(HTTPResponse response, HTTPd& parent)> func );
        DLL_EXPORT Error SetFileHandler( std::string extension, std::function<HTTPResponse(HTTPResponse response, HTTPd& parent, std::string localpath)> func );
        DLL_EXPORT Error SetDefaultIndex( std::string value );
        DLL_EXPORT HTTPd(std::string address, int port);
        DLL_EXPORT ~HTTPd();
        DLL_EXPORT Error Run();
        DLL_EXPORT HTTPResponse PushPage( HTTPd::HTTPResponse r, std::string mount = "" );
        DLL_EXPORT static std::string URLDecode(std::string url);

        DLL_EXPORT static HTTPResponse DoError(HTTPResponse response, HTTPd& parent, int code);


        #ifdef RunUnitTests
        DLL_EXPORT static bool RunTests();
        #endif

    };
}

#endif
