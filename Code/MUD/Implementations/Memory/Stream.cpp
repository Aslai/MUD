#include "Memory/Stream.hpp"
#include<string>
#include<cstdlib>
#include<cstring>

namespace GlobalMUD{

    void Stream::PushCheckpoint( size_t idx ){
        checkpoints.push_back( idx );
        if( idx < earliestCheckpoint )
            earliestCheckpoint = idx;
        Optimize( );
    }

    void Stream::PopCheckpoint( size_t idx ){
        if( idx == earliestCheckpoint ){
            size_t e = -1;
            for( unsigned int i = 0; i < checkpoints.size(); ++i ){
                if( checkpoints[i] == idx ){
                    checkpoints.erase( checkpoints.begin() + i );
                    idx = -1;
                    --i;
                    continue;
                }
                if( checkpoints[i] < e ){
                    e = checkpoints[i];
                }
            }
            earliestCheckpoint = e;
        }
        else{
            for( unsigned int i = 0; i < checkpoints.size(); ++i ){
                if( checkpoints[i] == idx ){
                    checkpoints.erase( checkpoints.begin() + i );
                    break;
                }
            }
        }
        Optimize( );
    }


    void Stream::GoToCheckpoint( size_t idx ){
        readPosition = idx;
        Optimize( );
    }

    void Stream::Optimize( ){
        if( earliestCheckpoint - offset > 5000 ){
            size_t change = earliestCheckpoint - offset;
            memcpy( buffer+earliestCheckpoint-offset, buffer, bufferend-earliestCheckpoint );
            offset += change;
            reserved += change;
        }
    }



    Stream::Checkpoint::Checkpoint( Stream* s, size_t idx ){
        stream = s;
        index = idx;
        stream->PushCheckpoint( idx );
    }


    Stream::Checkpoint::~Checkpoint(){
        if( stream != NULL )
            stream->PopCheckpoint( index );
    }


    Error Stream::Checkpoint::Remove(){

        if( stream != NULL ){
            stream->PopCheckpoint( index );
            stream = NULL;
        }
        return Error::None;
    }




    Stream::Stream(){
        buffer = NULL;
        reserved = 0;
        bufferend = 0;
        readPosition = 0;
        earliestCheckpoint = -1;
        offset = 0;
    }


    Stream::~Stream(){
        if( buffer )
            free( buffer );
    }


    void* Stream::GetBuffer( size_t length ){
        if( bufferend + length < reserved ){
            return buffer + bufferend - offset ;
        }
        else{
            reserved = bufferend + length + 2000;
            buffer = (char*) realloc( buffer, reserved - offset  );
            return buffer + bufferend - offset ;
        }
    }


    Error Stream::CommitBuffer( size_t length ){
        if( bufferend + length <= reserved ){
            bufferend += length;
        }
        else
            return Error::OutOfBounds;
        return Error::None;
    }


    Error Stream::PushBuffer( void* data, size_t length ){
        void *writeto = GetBuffer( length );
        memcpy( writeto, data, length );
        CommitBuffer( length );
        return Error::None;
    }


    bool Stream::HasLine(){
        for( size_t i = readPosition; i < bufferend; ++i ){
            if( buffer[i - offset] == '\n' )
                return true;
        }
        return false;
    }


    bool Stream::HasChar(){
        return readPosition < bufferend;
    }


    std::string Stream::PeekLine(){
        std::string ret = "";
        size_t i = readPosition;
        for( ; i < bufferend; ++i ){
            if( buffer[i - offset] == '\n' )
                break;
        }
        if( buffer[i - offset ] == '\n' ){
            ret.resize( i - readPosition );
            memcpy( &ret[0], buffer+readPosition - offset , i - readPosition );
        }
        return ret;
    }


    int Stream::PeekChar(){
        if( readPosition < bufferend )
            return buffer[readPosition - offset];
        return 1000;
    }

    std::string Stream::GetLine(){
        std::string ret = "";
        size_t i = readPosition;
        for( ; i < bufferend; ++i ){
            if( buffer[i - offset] == '\n' )
                break;
        }
        if( buffer[i - offset] == '\n' ){
            ret.resize( i - readPosition );
            memcpy( &ret[0], buffer+readPosition - offset , i - readPosition );
        }
        readPosition = i + 1;
        Optimize( );
        return ret;
    }

    int Stream::GetChar(){
        int chr = PeekChar();
        if( chr != 1000 )
            readPosition++;
        Optimize( );
        return chr;
    }

    Stream::Checkpoint Stream::SaveCheckpoint(){
        Checkpoint chk( this, readPosition );
        return chk;
    }

    Error Stream::LoadCheckpoint( Stream::Checkpoint point ){
        GoToCheckpoint( point.index );
        return Error::None;
    }

}

