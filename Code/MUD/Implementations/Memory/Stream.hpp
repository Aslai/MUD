#ifndef MUD_MEMORY_STREAM_HPP
#define MUD_MEMORY_STREAM_HPP

#include<string>
#include<vector>
#include<cstring>
#include<cstdlib>
#include "Error/Error.hpp"

namespace GlobalMUD{
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
            Checkpoint( Stream* s, size_t idx );
            ~Checkpoint();
            Error Remove();
            size_t operator-(Checkpoint other);
            Checkpoint& operator++();
            Checkpoint& operator++(int);


        };
        friend Checkpoint;

        Stream();
        ~Stream();
        void* GetBuffer( size_t length );
        Error CommitBuffer( size_t length );
        Error PushBuffer( const void* data, size_t length );

        Checkpoint End();

        bool HasLine();
        template<class... Args>
        bool HasToken( Args... args ){
            std::vector<std::string> t = {args...};
            Token tok = TokenEnd( t );
            return tok.pos != std::string::npos;
        }
        bool HasChar();
        bool HasByte();
        bool HasData( Checkpoint end );

        std::string PeekLine();
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

        int PeekChar();
        byte PeekByte();
        void* PeekData( Checkpoint end );

        std::string GetLine();
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
        int GetChar();
        byte GetByte();
        void* GetData( Checkpoint end );

        Checkpoint SaveCheckpoint();
        Error LoadCheckpoint( Checkpoint point );
    };
}

#endif
