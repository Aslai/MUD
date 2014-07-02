#ifndef MUD_MEMORY_STREAM_HPP
#define MUD_MEMORY_STREAM_HPP

#include<string>
#include<vector>
#include "Error/Error.hpp"

namespace GlobalMUD{
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

    public:
        class Checkpoint{
            friend Stream;
            size_t index;
            Stream* stream;
        public:
            Checkpoint( Stream* s, size_t idx );
            ~Checkpoint();
            Error Remove();
        };
        friend Checkpoint;

        Stream();
        ~Stream();
        void* GetBuffer( size_t length );
        Error CommitBuffer( size_t length );
        Error PushBuffer( void* data, size_t length );
        bool HasLine();
        bool HasChar();
        std::string PeekLine();
        int PeekChar();
        std::string GetLine();
        int GetChar();
        Checkpoint SaveCheckpoint();
        Error LoadCheckpoint( Checkpoint point );
    };
}

#endif
