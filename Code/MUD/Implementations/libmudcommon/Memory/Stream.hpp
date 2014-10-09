#ifndef MUD_MEMORY_STREAM_HPP
#define MUD_MEMORY_STREAM_HPP

#include "libmudcommon/dllinfo.hpp"

#include<string>
#include<vector>
#include<cstring>
#include<cstdlib>
#include "libmudcommon/Error/Error.hpp"

namespace MUD{
    typedef unsigned char byte;

    class Stream{
        char* buffer;
        size_t reserved;
        size_t bufferend;
        size_t readPosition;
        size_t offset;
        size_t earliestCheckpoint;

        std::vector<size_t> checkpoints;
        void PushCheckpoint( size_t idx );
        void PopCheckpoint( size_t idx );
        void GoToCheckpoint( size_t idx );
        void Optimize();

        struct Token{
            size_t pos;
            std::string delimiter;
        };
        Token TokenEnd( std::vector<std::string> delimiters );



    public:
        class Checkpoint{
            friend Stream;
            size_t index;
            Stream* stream;
        public:
            DLL_EXPORT Checkpoint( Stream* s, size_t idx );
            DLL_EXPORT ~Checkpoint();
            DLL_EXPORT Error Remove();
            DLL_EXPORT size_t operator-(Checkpoint other);
            DLL_EXPORT Checkpoint& operator++();
            DLL_EXPORT Checkpoint& operator++(int);


        };
        friend Checkpoint;

        DLL_EXPORT Stream();
        DLL_EXPORT ~Stream();
        DLL_EXPORT void* GetBuffer( size_t length );
        DLL_EXPORT Error CommitBuffer( size_t length );
        DLL_EXPORT Error PushBuffer( const void* data, size_t length );

        DLL_EXPORT Checkpoint End();

        bool HasLine();
        template<class... Args>
        bool HasToken( Args... args ){
            std::vector<std::string> t = {args...};
            Token tok = TokenEnd( t );
            return tok.pos != std::string::npos;
        }
        DLL_EXPORT bool HasChar();
        DLL_EXPORT bool HasByte();
        DLL_EXPORT bool HasData( Checkpoint end );

        DLL_EXPORT std::string PeekLine();
        template<class... Args>
        std::string PeekToken( Args... args ){
            std::vector<std::string> t = {args...};

            Token tok = TokenEnd( t );
            size_t end = tok.pos;
            std::string ret = "";
            if( end != std::string::npos ){
                ret.resize( end - readPosition );
                memcpy( &ret[0], buffer+readPosition - offset , end - readPosition );
            }
            return ret;
        }

        DLL_EXPORT int PeekChar();
        DLL_EXPORT byte PeekByte();
        DLL_EXPORT void* PeekData( Checkpoint end );

        DLL_EXPORT std::string GetLine();
        template<class... Args>
        std::string GetToken( Args... args ){
            std::vector<std::string> t = {args...};

            Token tok = TokenEnd( t );
            size_t end = tok.pos;
            std::string ret = "";
            if( end != std::string::npos ){
                ret.resize( end - readPosition );
                memcpy( &ret[0], buffer+readPosition - offset , end - readPosition );
                readPosition = end + tok.delimiter.length();
                Optimize();
            }

            return ret;
        }
        DLL_EXPORT int GetChar();
        DLL_EXPORT byte GetByte();
        DLL_EXPORT void* GetData( Checkpoint end );
        DLL_EXPORT std::string GetString( Checkpoint end );

        DLL_EXPORT Checkpoint SaveCheckpoint();
        DLL_EXPORT Error LoadCheckpoint( Checkpoint point );
    };
}

#endif
