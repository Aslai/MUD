#ifndef MUD_HTTPD_HTTPD_HPP
#define MUD_HTTPD_HTTPD_HPP


#include<map>
#include<string>
#include "Global/Error.hpp"
#include "CommStream/CommStream.hpp"
#include "Global/Thread.hpp"

#undef ErrorRoot
#define ErrorRoot 300
namespace GlobalMUD{
    namespace ERROR{
        Error MountFailure = ErrorRoot + 0;
        Error MountExists = ErrorRoot + 1;
        Error Timeout = ErrorRoot + 2;

    }
    class HTTPd{
    public:
        struct HTTPResponse{
            int error;
            std::map<std::string,std::string> headers;
            std::map<std::string,std::string> gets;
            std::map<std::string,std::string> posts;
            std::string request;
            std::string filepath;
            std::vector<char> content;
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
            HTTPResponse(*Func)(HTTPResponse response, HTTPd& parent);
        };
    private:
        std::map<std::string, MountPoint> MountPoints;
        std::string Address;
        int Port;
        CommStream *stream;
        ThreadMember *MyThread;
        static void ConnectionHandler(CommStream stream, void* argument );
        static HTTPResponse Do400(HTTPResponse response, HTTPd& parent);
        static HTTPResponse Do403(HTTPResponse response, HTTPd& parent);
        static HTTPResponse Do404(HTTPResponse response, HTTPd& parent);
        static HTTPResponse Do408(HTTPResponse response, HTTPd& parent);


    public:
        Error MountFolder(std::string mountpath, std::string folderpath);
        Error MountDirectory(std::string mountpath, std::string folderpath);
        Error MountFile(std::string mountpath, std::string filepath);
        Error MountFunction(std::string mountpath, HTTPResponse(*func)(HTTPResponse response, HTTPd& parent));
        HTTPd(std::string address, int port);
        ~HTTPd();
        Error Run();
        HTTPResponse PushPage( HTTPd::HTTPResponse r, std::string mount = "" );
        static std::string ReadWord(char*&pointer, size_t&len);
        static std::string ReadLine(char*&pointer, size_t&len);
        static int HasWord(char*pointer, size_t len);
        static int HasLine(char*pointer, size_t len);
        static std::string URLDecode(std::string url);
        #ifdef RunUnitTests
        static bool RunTests();
        #endif

    };
}

#endif
