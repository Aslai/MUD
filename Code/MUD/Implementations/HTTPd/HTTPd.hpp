#ifndef MUD_HTTPD_HTTPD_HPP
#define MUD_HTTPD_HTTPD_HPP


#include<map>
#include<string>
#include "Error/Error.hpp"
#include "CommStream/CommStream.hpp"
#include "Thread/Thread.hpp"
#include "Memory/Stream.hpp"

namespace GlobalMUD{
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
            void SetContent(std::string value);
            HTTPResponse& operator+=(HTTPResponse other);
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
        Error MountFolder(std::string mountpath, std::string folderpath);
        Error MountDirectory(std::string mountpath, std::string folderpath);
        Error MountFile(std::string mountpath, std::string filepath);
        Error MountFunction(std::string mountpath, std::function<HTTPResponse(HTTPResponse response, HTTPd& parent)> func );
        Error SetFileHandler( std::string extension, std::function<HTTPResponse(HTTPResponse response, HTTPd& parent, std::string localpath)> func );
        Error SetDefaultIndex( std::string value );
        HTTPd(std::string address, int port);
        ~HTTPd();
        Error Run();
        HTTPResponse PushPage( HTTPd::HTTPResponse r, std::string mount = "" );
        static std::string URLDecode(std::string url);

        static HTTPResponse DoError(HTTPResponse response, HTTPd& parent, int code);


        #ifdef RunUnitTests
        static bool RunTests();
        #endif

    };
}

#endif
