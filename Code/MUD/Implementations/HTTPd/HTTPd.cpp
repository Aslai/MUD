#include<map>
#include<string>
#include<cstring>
#include "Global/Error.hpp"
#include "Global/Thread.hpp"
#include "HTTPd/HTTPd.hpp"
#include "CommStream/CommStream.hpp"
#include "Global/Filesystem.hpp"
#include "HTTPd/MIME.hpp"
#include "HTTPd/HTTPStatus.hpp"

#include <cctype>
#include <ctime>

#undef ErrorRoot
#define ErrorRoot 300
namespace GlobalMUD{
        static std::string StringToLower(std::string in){
            for( size_t i = 0; i < in.size(); ++i ){
                in[i] = tolower(in[i]);
            }
            return in;
        }
        /*static std::string StringFromInt(int i){
            char s[30];
            snprintf(s, 29, "%d", i );
            return s;
        }*/
        static std::string StringFromUInt(unsigned int i){
            char s[30];
            snprintf(s, 29, "%u", i );
            return s;
        }

        std::string HTTPd::ReadWord(char*&pointer, size_t&len){
            std::string ret = "";
            while( len > 0 && (*pointer == ' ' || *pointer == '\t')){
                pointer++;
                len--;
            }
            while( len > 0 && !isspace( *pointer )){
                    ret += *pointer;
                pointer++;
                len--;
            }
            return ret;
        }
        std::string HTTPd::ReadLine(char*&pointer, size_t&len){
            std::string ret = "";
            while( len > 0 && *pointer != '\n'){
                ret += *pointer;
                pointer++;
                len--;
            }
            return ret;
        }
        int HTTPd::HasWord(char*pointer, size_t len){
            while( len > 0 && (*pointer == ' ' || *pointer == '\t')){
                pointer++;
                len--;
            }
            while( len > 0 && !isspace( *pointer )){
                pointer++;
                len--;
            }
            return len != 0;
        }
        int HTTPd::HasLine(char*pointer, size_t len){
            while( len > 0 && *pointer != '\n'){
                pointer++;
                len--;
            }
            return len != 0;
        }
        std::string HTTPd::URLDecode(std::string url){
            std::string ret = "";
            char escape;
            bool escaped = false;
            int escapelevel = 0;
            bool valid = false;
            for( unsigned int i = 0; i < url.length(); ++i ){
                if( escaped ){
                    char c = tolower( url[i] );
                    int v = 0;
                    if( c >= '0' && c <='9' ){
                        v = c - '0';
                    }
                    else if( c >= 'a' && c <='f' ){
                        v = c - 'a' + 10;
                    }
                    else{
                        valid = false;
                    }
                    if( escapelevel == 0 ){
                        escape += v << 4;
                        escapelevel++;
                    }
                    else{
                        escape += v;
                        if( valid )
                            ret += escape;
                        escaped = false;
                    }
                }
                else {
                    if( url[i] == '+' ){
                        ret += ' ';
                    }
                    else if( url[i] == '%'){
                        escaped = true;
                        escape = 0;
                        escapelevel = 0;
                        valid = true;
                    }
                    else {
                        ret += url[i];
                    }
                }
            }
            return ret;
        }
        #ifdef RunUnitTests
        static HTTPd::HTTPResponse funfunc(HTTPd::HTTPResponse r, HTTPd& parent ){
            HTTPd::HTTPResponse ret;
            ret.error = 200;
            std::string t = "Testing";
            ret.content.resize( t.size() );
            memcpy( &(ret.content[0]), t.c_str(), t.length() );

            return ret;
        }

        bool HTTPd::RunTests(){
            TEST("GlobalMUD::HTTPd");
            HTTPd server("any", 45141);
            server.MountFunction("/api/test", funfunc );
             server.MountFolder("/", "C:\\Users\\Kaslai\\Downloads\\timcook\\");
            //server.MountFolder("/", "testsite/");
            //server.MountFile("/music.mp3", "E:\\Music\\CRISCY TUNEZ\\xc3.ogg");
            //server.MountFile("/stuff/index.html", "C:\\Users\\Kaslai\\Documents\\Documents\\Random text clips\\pasta\\mountaindew2018.txt");

            server.Run();
            return true;
        }
        #endif
        static void ConnectionHandlerThread(CommStream stream, void* parent){

            HTTPd::HTTPResponse r;
            size_t size = 1000;
            size_t myend = 0;
            char* buffer = (char*)malloc(size);
            char* bufferwrite = buffer;
            char* bufferread = buffer;
            size_t readremaining = 0;
            const time_t timeout = 45;
            std::string verb;

            Error myerror = Error::None;


            int state = 0;
            time_t starttime = time(0);

            while(state != -1){
                if( time(0) - starttime > timeout ){
                    //myerror = Error::Timeout;
                }
                Error e = Error::None;
                while( true ){
                    size_t mysize = size-myend;
                    e = stream.Receive( bufferwrite, mysize );
                    if( e == Error::NoData || e == Error::NotConnected )
                        break;
                    myend += mysize;
                    bufferwrite += mysize;
                    if( myend > size/2 ){
                        size *= 2;
                        int bufferwriteoffset = bufferwrite - buffer;
                        int bufferreadoffset = bufferread - buffer;

                        buffer = (char*) realloc(buffer, size);
                        bufferwrite = buffer + bufferwriteoffset;
                        bufferread = buffer + bufferreadoffset;
                    }
                }
                if( e == Error::NotConnected )
                    break;
                readremaining = myend-(bufferread-buffer);
                switch(state){
                    case 0:
                        if( HTTPd::HasWord (bufferread, readremaining) ){
                            verb = HTTPd::ReadWord(bufferread, readremaining);
                        }
                        else break;
                    case 1: state = 1;
                        if( HTTPd::HasWord (bufferread, readremaining) ){
                            r.request = HTTPd::ReadWord(bufferread, readremaining);
                            size_t pos = r.request.find_first_of('?');
                            if( pos == std::string::npos ){
                                r.request = HTTPd::URLDecode( r.request );
                            }
                            else{
                                std::string params = r.request.substr(pos+1);
                                r.request = HTTPd::URLDecode( r.request.substr(0,pos) );
                                pos = 0;
                                while( true ){
                                    size_t oldpos = pos;
                                    pos = params.find_first_of('&', pos);
                                    std::string entry = params.substr(oldpos, pos-oldpos);

                                    oldpos = entry.find_first_of('=');
                                    std::string id = "";
                                    if( oldpos != std::string::npos ){
                                        id = HTTPd::URLDecode(entry.substr(0, oldpos));
                                        for( size_t i = 0; i < id.size(); ++i ){
                                            id[i] = tolower(id[i]);
                                        }
                                        entry = HTTPd::URLDecode(entry.substr(oldpos+1));
                                        r.gets[id] = entry;
                                    }

                                    if( pos == std::string::npos ){
                                        break;
                                    }
                                    pos++;
                                }
                            }
                            pos = 0;
                            std::vector<std::string> dir;
                            do{
                                size_t oldpos = pos+1;
                                pos = r.request.find_first_of('/', oldpos);
                                dir.push_back( r.request.substr(oldpos, pos-oldpos) );
                            }while( pos != std::string::npos );

                            for( size_t i = 0; i < dir.size(); ++i ){
                                if( dir[i] == ".." ){
                                    dir.erase(dir.begin()+i);
                                    if( i != 0 ){
                                        dir.erase(dir.begin()+i-1);
                                    }
                                    i -= 2;
                                }
                                else if( dir[i] == "." ){
                                    dir.erase(dir.begin()+i);
                                    i--;
                                }
                                else if( dir[i] == "" ){
                                    if( i < dir.size()-1 ){
                                    dir.erase(dir.begin()+i);
                                    i--;
                                    }
                                    else{
                                        dir[i] = "index.html";
                                    }
                                }

                            }
                            r.request = "";
                            for( size_t i = 0; i < dir.size(); ++i ){
                                r.request += "/"+dir[i];
                            }
                            if( r.request == "/" || r.request == "" )
                                r.request = "/index.html";

                        }
                        else break;

                    case 2: state = 2;
                        if( HTTPd::HasWord (bufferread, readremaining) ){
                             std::string tmp = HTTPd::ReadWord(bufferread, readremaining);

                             if( StringToLower(tmp.substr(0,5)) != "http/" ){
                                myerror = Error::ParseFailure;
                                state = -1;
                                break;
                             }
                        }
                        else break;
                        HTTPd::ReadLine(bufferread, readremaining);
                    case 3: state = 3;
                        while( HTTPd::HasLine(bufferread, readremaining) ){
                            std::string tmp = HTTPd::ReadLine(bufferread, readremaining);
                            if( tmp == "" ){
                                break;
                            }
                            size_t pos = tmp.find_first_of(':');
                            if( pos != std::string::npos ){
                                std::string id = tmp.substr(0, pos);
                                while( tmp[pos] == ' ' && pos < tmp.size() ){
                                    pos++;
                                }
                                r.headers[StringToLower(id)] = tmp.substr(pos);
                            }
                        }
                    case 4: state = 4;
                        if( r.headers["content-length"] != "" ){
                            size_t len = atol( r.headers["content-length"].c_str() );
                            if( len <= readremaining ){
                                r.content.resize( len );
                                memcpy( &(r.content[0]), bufferread, len );
                            }
                            else break;
                        }
                    case 5: state = -1;

                }
            }

            HTTPd::HTTPResponse tosend;
            if( myerror == Error::ParseFailure ){
                tosend = HTTPd::Do400(r, *(HTTPd*)parent);
            }
            else if( myerror == Error::Timeout ){
                tosend = HTTPd::Do408(r, *(HTTPd*)parent);
            }
            else if( myerror == Error::None ){
                tosend = ((HTTPd*)parent)->PushPage(r);
            }

            stream.Send("HTTP/1.1 " + StringFromUInt(tosend.error) + " " + GetMessageFromHTTPStatus(tosend.error) + "\r\n", false);
            tosend.headers["Connection"] = "Close";

            std::map<std::string,std::string>::iterator iter = tosend.headers.begin();


            while( iter != tosend.headers.end()){
                stream.Send( iter->first + ": " + iter->second + "\r\n", false );
                iter++;
            }
            stream.Send("\r\n", false);

            if( tosend.filepath != "" ){
                stream.SendFile( tosend.filepath, false );
            }
            else{
                stream.Send( &(tosend.content[0]), tosend.content.size(), false );
            }
            //Thread::Sleep(1000);
            stream.Disconnect();
            free(buffer);


            return;
        }
        void HTTPd::ConnectionHandler(CommStream stream, void* parent ){

            Thread T( std::bind( ConnectionHandlerThread, stream, parent ) );

            T.Detach();
            T.Run();

        }


        HTTPd::HTTPResponse HTTPd::Do400(HTTPResponse response, HTTPd& parent){

            HTTPd::HTTPResponse r;
            r = parent.PushPage(r, "e400");
            if( r.error == 404 ){
                std::string html = "<!DOCTYPE html><html><head><title>Error 400 - Bad Request</title></head>\
                            <body><h1>400 - Bad Request</h1><br/><p>Your request could not be parsed.</p><br/>\
                            <br/><p>Additionally, a 404 was encountered when attempting to retrieve the error document.</p></body></html>";
                r.content.resize(html.size());

                memcpy( &(r.content[0]), html.c_str(), html.length() );
            }
            r.error = 400;
            return r;
        }

        HTTPd::HTTPResponse HTTPd::Do403(HTTPResponse response, HTTPd& parent){

            HTTPd::HTTPResponse r;
            r = parent.PushPage(r, "e403");
            if( r.error == 404 ){
                std::string html = "<!DOCTYPE html><html><head><title>Error 403 - Forbidden</title></head>\
                            <body><h1>403 - Forbidden</h1><br/><p>The requested resource is forbidden from the likes of you.</p><br/>\
                            <br/><p>Additionally, a 404 was encountered when attempting to retrieve the error document.</p></body></html>";
                r.content.resize(html.size());

                memcpy( &(r.content[0]), html.c_str(), html.length() );
            }
            r.error = 403;
            return r;
        }

        HTTPd::HTTPResponse HTTPd::Do404(HTTPResponse response, HTTPd& parent){

            HTTPd::HTTPResponse r;
            r = parent.PushPage(r, "e404");
            if( r.error == 404 ){
                std::string html = "<!DOCTYPE html><html><head><title>Error 404 - Not Found</title></head>\
                            <body><h1>404 - Not Found</h1><br/><p>The requested resource could not be found.</p><br/>\
                            <br/><p>Additionally, a 404 was encountered when attempting to retrieve the error document.</p></body></html>";
                r.content.resize(html.size());

                memcpy( &(r.content[0]), html.c_str(), html.length() );
            }
            r.error = 404;
            return r;
        }

        HTTPd::HTTPResponse HTTPd::Do408(HTTPResponse response, HTTPd& parent){

            HTTPd::HTTPResponse r;
            r = parent.PushPage(r, "e408");
            if( r.error == 404 ){
                std::string html = "<!DOCTYPE html><html><head><title>Error 408 - Request Timeout</title></head>\
                            <body><h1>408 - Request Timeout</h1><br/><p>The request for this resource has timed out.</p><br/>\
                            <br/><p>Additionally, a 404 was encountered when attempting to retrieve the error document.</p></body></html>";
                r.content.resize(html.size());

                memcpy( &(r.content[0]), html.c_str(), html.length() );
            }
            r.error = 408;
            return r;
        }



        Error HTTPd::MountFolder(std::string mountpath, std::string folderpath){
            return MountDirectory( mountpath, folderpath );
        }

        HTTPd::HTTPResponse HTTPd::PushPage(HTTPd::HTTPResponse r, std::string mount2 ){
            std::string mount = mount2;
            if( mount == "" ){
                std::string test = StringToLower(r.request);
                size_t pos = std::string::npos;
                while( test != "" ){
                    test = test.substr( 0, pos );
                    if( test == "" )
                        test = "/";
                    if( MountPoints[test].Type != HTTPd::MountPoint::BAD ){
                        mount = test;
                        break;
                    }
                    if( test == "/" )
                        break;
                    pos = test.find_last_of('/');
                }
            }
            if( mount == "" || (mount[0] != 'e' && mount != StringToLower(r.request) && MountPoints[mount].Type != HTTPd::MountPoint::FOLDER ) ){
                return Do404(r, *this);
            }
            HTTPd::HTTPResponse ret;
            if( mount[0] == 'e' && MountPoints[mount].Type == HTTPd::MountPoint::BAD ){
                ret.error = 404;
                return ret;
            }
            ret.error = 200;
            std::string path = MountPoints[mount].Path;
            switch( MountPoints[mount].Type ){
            case HTTPd::MountPoint::FOLDER:
                path = MountPoints[mount].Path + r.request.substr(mount.length());
            case HTTPd::MountPoint::FILE:{
                if( !Filesystem::FileExists( path ) ){
                    return Do404(r, *this);
                }

                ret.headers["Content-Length"] = StringFromUInt(Filesystem::FileSize( path ));
                size_t pos = path.find_last_of('.');

                if( path.find_first_of('.') != std::string::npos ){
                    ret.headers["Content-Type"] = GetMimeFromExt(path.substr(pos==std::string::npos?pos:pos+1));
                }
                ret.filepath = path;
                return ret;
            }
            break;

            case HTTPd::MountPoint::FUNCTION:{
                ret = MountPoints[mount].Func(r, *this);
                return ret;
            }
            case HTTPd::MountPoint::BAD:
                return Do404(r,*this);
            default:
                return Do404(r,*this);
            break;
            }
        }

        Error HTTPd::MountDirectory(std::string mountpath, std::string folderpath){
            MountPoint m;
            m.Type = MountPoint::FOLDER;
            m.Path = folderpath;
            m.MountPath = StringToLower(mountpath);
            MountPoints[StringToLower(mountpath)] = m;
            return Error::None;
        }

        Error HTTPd::MountFile(std::string mountpath, std::string filepath){
            MountPoint m;
            m.Type = MountPoint::FILE;
            m.Path = filepath;
            m.MountPath = StringToLower(mountpath);
            MountPoints[StringToLower(mountpath)] = m;
            return Error::None;
        }

        Error HTTPd::MountFunction(std::string mountpath, HTTPResponse(*func)(HTTPResponse response, HTTPd& parent)){
            MountPoint m;
            m.Type = MountPoint::FUNCTION;
            m.Func = func;
            m.MountPath = StringToLower(mountpath);
            MountPoints[StringToLower(mountpath)] = m;
            return Error::None;
        }

        HTTPd::HTTPd(std::string address, int port){
            Address = address;
            Port = port;
            stream = 0;
            MyThread = 0;
        }

        HTTPd::~HTTPd(){
            MyThread->Join();
            if( MyThread != 0 ){
                delete MyThread;
            }
            if( stream != 0 ){
                delete stream;
            }
        }

        Error HTTPd::Run(){
            stream = new CommStream(CommStream::BINARY);
            MyThread = new Thread(  std::bind( &CommStream::ListenOn, stream, Address, Port,
                                        (std::function<void(CommStream)>)(std::bind(ConnectionHandler, std::placeholders::_1, (void*)this )) ) );
            MyThread->Run();

            //stream->ListenOn( Address, Port, ConnectionHandler, (void*)this );
            return Error::None;
        }

}
