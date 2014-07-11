#include<map>
#include<string>
#include<cstring>
#include "Error/Error.hpp"
#include "Thread/Thread.hpp"
#include "HTTPd/HTTPd.hpp"
#include "CommStream/CommStream.hpp"
#include "Filesystem/Filesystem.hpp"
#include "Filesystem/MIME.hpp"
#include "HTTPd/HTTPStatus.hpp"
#include "Strings/Strings.hpp"
#include "Memory/Stream.hpp"

#include <cctype>
#include <ctime>

#undef ErrorRoot
#define ErrorRoot 300
namespace GlobalMUD{
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
                    //Convert a hexadecimal glyph into a numeric value and store for later.
                    if( c >= '0' && c <='9' ){
                        v = c - '0';
                    }
                    else if( c >= 'a' && c <='f' ){
                        v = c - 'a' + 10;
                    }
                    else{
                        valid = false;
                    }
                    //If this is the first glyph in the escape sequence...
                    if( escapelevel == 0 ){
                        escape += v << 4;
                        escapelevel++;
                    }
                    else{
                        //otherwise generate a character from the two hexadecimal values and
                        //dump it into the output.
                        escape += v;
                        if( valid )
                            ret += escape;
                        escaped = false;
                    }
                }
                else {
                    //Convert pluses to spaces
                    if( url[i] == '+' ){
                        ret += ' ';
                    }
                    //Drop into escape mode if a percent is present
                    else if( url[i] == '%'){
                        escaped = true;
                        escape = 0;
                        escapelevel = 0;
                        valid = true;
                    }
                    //Otherwise just dump the character as-is into the output
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
            ret.status = 200;
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

            server.Run();
            return true;
        }
        #endif

        void HTTPd::ConnectionHandlerThread(CommStream stream, void* parent){
            Stream data;
            HTTPd::HTTPResponse r;
            const time_t timeout = 10;
            std::string verb;
            int failpenalty = 1;

            Error myerror = Error::None;


            int state = 0;
            time_t starttime = time( nullptr );

            while(state != -1){
                //If the user has exceeded the maximum allotted timeout, kill the connection.
                if( time( nullptr ) - starttime > timeout ){
                    myerror = Error::Timeout;
                    break;
                }
                Error e = Error::None;
                while( true ){
                    //Fill the Stream with data from CommStream
                    size_t mysize = 1999;
                    char* buffer = (char*)data.GetBuffer(mysize);
                    e = stream.Receive( buffer, mysize );
                    data.CommitBuffer( mysize );
                    if( mysize == 0 ){
                        Thread::Sleep(failpenalty);
                        failpenalty *= 2;
                    }
                    if( e == Error::NoData || e == Error::NotConnected )
                        break;
                }
                if( e == Error::NotConnected )
                    break;

                switch(state){
                    case 0:
                        //Get the HTTP verb (GET, POST, etc)
                        if( data.HasToken(" ", "\r\n") ){
                            verb = data.GetToken(" ", "\r\n");
                        }
                        else break;
                    case 1: state = 1;
                        //Parse GET parameters, unescape, and sanitize . and .. in the url
                        if( data.HasToken(" ", "\r\n") ){
                            r.request = HTTPd::ParseRequest( data, r );

                        }
                        else break;
                        if(data.PeekToken("\r\n").length() <= 2 )
                            data.GetToken( "\r\n" );
                    case 2: state = 2;
                        //Verify that the http/1.x line is present
                        if( data.HasToken(" ", "\r\n") ){
                             std::string tmp = data.GetToken(" ", "\r\n");

                             if( StringToLower(tmp.substr(0,5)) != "http/" ){
                                myerror = Error::ParseFailure;
                                state = -1;
                                break;
                             }
                        }
                        else break;
                        data.GetLine();
                    case 3: state = 3;
                        while( data.HasLine() ){
                            //Extract the headers from the stream
                            std::string tmp = data.GetLine();
                            if( tmp == "" ){
                                break;
                            }
                            size_t pos = tmp.find_first_of(':');
                            if( pos != std::string::npos ){
                                std::string id = tmp.substr(0, pos);
                                while( tmp[pos] == ' ' && pos < tmp.size() ){
                                    pos++;
                                }
                                r.headers[StringToLower(id)] = tmp.substr(pos,tmp.find_first_of('\r')-1);
                            }
                        }
                    case 4: state = 4;
                        //Finish reading data from the stream, and stick it in the response object.
                        if( r.headers["content-length"] != "" ){
                            size_t len = atol( r.headers["content-length"].c_str() );
                            auto end = data.End();
                            if( data.HasData( end ) ){
                                auto start = data.SaveCheckpoint();
                                if( len > end - start )
                                    len = end - start;

                                r.content.resize( len );
                                memcpy( &(r.content[0]), data.GetData( end ), len );
                            }
                            else break;
                        }
                    case 5: state = -1;
                    default: break;
                }
            }

            HTTPd::HTTPResponse tosend;
            if( myerror == Error::ParseFailure ){
                tosend = HTTPd::DoError(r, *(HTTPd*)parent, 400);
            }
            else if( myerror == Error::Timeout ){
                tosend = HTTPd::DoError(r, *(HTTPd*)parent, 408);
            }
            else if( myerror == Error::None ){
                //If all went well, push relevant headers into the HTTPResponse
                tosend = ((HTTPd*)parent)->PushPage(r);
            }

            //Begin responding with an HTTP response
            stream.Send("HTTP/1.1 " + StringFromUInt(tosend.status) + " " + GetMessageFromHTTPStatus(tosend.status) + "\r\n", false);
            tosend.headers["Connection"] = "Close";

            std::map<std::string,std::string>::iterator iter = tosend.headers.begin();

            //Send all the headers
            while( iter != tosend.headers.end()){
                stream.Send( iter->first + ": " + iter->second + "\r\n", false );
                iter++;
            }
            //Finished with headers. Begin sending data.
            stream.Send("\r\n", false);

            if( tosend.filepath != "" ){
                stream.SendFile( tosend.filepath, false );
            }
            else{
                stream.Send( &(tosend.content[0]), tosend.content.size(), false );
            }
            //Thread::Sleep(1000);
            stream.Disconnect();

            return;
        }

        std::string HTTPd::ParseRequest(Stream &data, HTTPResponse &r){
            //Get the request string
            r.request = data.GetToken( " ", "\r\n" );
            size_t pos = r.request.find_first_of('?');
            if( pos == std::string::npos ){
                //If there are no GET parameters, just decode the entire string
                r.request = HTTPd::URLDecode( r.request );
            }
            else{
                std::string params = r.request.substr(pos+1);
                //Decode everything preceding the parameters.
                r.request = HTTPd::URLDecode( r.request.substr(0,pos) );
                pos = 0;
                while( true ){
                    size_t oldpos = pos;
                    pos = params.find_first_of('&', pos);
                    std::string entry = params.substr(oldpos, pos-oldpos);

                    oldpos = entry.find_first_of('=');
                    std::string id = "";
                    if( oldpos != std::string::npos ){
                        //Once we've found a parameter, URLDecode them into the HTTP header
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
            //Split the request string up into a series of strings. One string per level of the directory.
            do{
                size_t oldpos = pos+1;
                pos = r.request.find_first_of('/', oldpos);
                dir.push_back( r.request.substr(oldpos, pos == std::string::npos ? pos : pos-oldpos) );
            }while( pos != std::string::npos );

            //Convert instances of .. and . into what the actual directory will be by removing directories
            //for instances of ..
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
                        //If this is a request for an empty directory, point it at index.html
                        dir[i] = "index.html";
                    }
                }

            }
            //Reconstruct a single string for the request, but all sanitized and with GET
            //parameters stripped off.
            r.request = "";
            for( size_t i = 0; i < dir.size(); ++i ){
                r.request += "/"+dir[i];
            }
            if( r.request == "/" || r.request == "" )
                r.request = "/index.html";
            return r.request;
        }

        void HTTPd::ConnectionHandler(CommStream stream, void* parent ){

            Thread T( std::bind( ConnectionHandlerThread, stream, parent ) );
            T.Run();
            T.Detach();

        }

        void HTTPd::HTTPResponse::SetContent(std::string value){
            content.resize(value.size());
            memcpy( &(content[0]), value.c_str(), value.length() );
        }


        HTTPd::HTTPResponse HTTPd::DoError(HTTPResponse response, HTTPd& parent, int code){
            //Return an error page.
            //If a registered error handler does not exist, return a generic page.
            HTTPd::HTTPResponse r;
            r = parent.PushPage(r, StringFormat( "e%d", code ));
            if( r.status == 404 ){
                std::string title = GetMessageFromHTTPStatus( code );
                std::string message = GetDetailedMessageFromHTTPStatus( code );
                r.SetContent( StringFormat(
                            "<!DOCTYPE html><html><head><title>%d - %s</title></head>\
                            <body><h1>%d - %s</h1>%s<br/>\
                            Additionally, a 404 was encountered when attempting to retrieve the error document.\
                            <hr align=\"LEFT\" width=\"100%\"><i>MUDHTTPd web server</i></body></html>",
                            code, title, code, title, message ) );
            }
            r.status = code;
            return r;
        }



        Error HTTPd::MountFolder(std::string mountpath, std::string folderpath){
            return MountDirectory( mountpath, folderpath );
        }

        HTTPd::HTTPResponse HTTPd::PushPage(HTTPd::HTTPResponse r, std::string mount2 ){
            std::string mount = mount2;
            //Find a mountpoint that forms part of the base directories for the request
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
            Print(mount);
            HTTPd::HTTPResponse ret;
            if( mount[0] == 'e' && MountPoints[mount].Type == HTTPd::MountPoint::BAD ){
                ret.status = 404;
                return ret;
            }

            //If no mount could be found, or it's a folder, 404
            if( mount == "" || (mount[0] != 'e' && mount != StringToLower(r.request) && MountPoints[mount].Type != HTTPd::MountPoint::FOLDER ) ){
                return DoError(r, *this, 404);
            }

            ret.status = 200;
            std::string path = MountPoints[mount].Path;
            switch( MountPoints[mount].Type ){
            //If the mount that we're working from is a folder, set path to the fully qualified
            //local path
            case HTTPd::MountPoint::FOLDER:
                path = MountPoints[mount].Path + r.request.substr(mount.length());
            //Fall through to file handling.
            //The next case is also used if the mountpoint is a single file.
            case HTTPd::MountPoint::FILE:{
                //404 if the file doesn't exist
                if( !Filesystem::FileExists( path ) ){
                    return DoError(r, *this, 404);
                }

                ret.headers["Content-Length"] = StringFromUInt(Filesystem::FileSize( path ));
                size_t pos = path.find_last_of('.');

                if( path.find_first_of('.') != std::string::npos ){
                    ret.headers["Content-Type"] = GetMimeFromExt(path.substr(pos==std::string::npos?pos:pos+1));
                }
                //Save the file path for sending at a different point.
                ret.filepath = path;
                return ret;
            }
            break;

            case HTTPd::MountPoint::FUNCTION:{
                //Call a mounted function if that's what the mount is
                ret = MountPoints[mount].Func(r, *this);
                return ret;
            }
            //In the case of a bad mount, 404
            case HTTPd::MountPoint::BAD:
               return DoError(r, *this, 404);
            default:
                return DoError(r, *this, 404);
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

        Error HTTPd::MountFunction(std::string mountpath, std::function<HTTPResponse(HTTPResponse response, HTTPd& parent)> func ){
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
            stream = nullptr;
            MyThread = nullptr;
        }

        HTTPd::~HTTPd(){
            MyThread->Join();
            if( MyThread != nullptr ){
                delete MyThread;
            }
            if( stream != nullptr ){
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
