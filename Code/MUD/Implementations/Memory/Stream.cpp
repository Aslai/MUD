#include "Memory/Stream.hpp"
#include<string>
#include<cstdlib>
#include<cstring>
#include<cstdio>
#include "Strings/Strings.hpp"

namespace GlobalMUD{

    void Stream::PushCheckpoint( size_t idx ){
        checkpoints.push_back( idx );
        //If the new index is earlier than the old earliest checkpoint, update the earliest checkpoint.
        if( idx < earliestCheckpoint )
            earliestCheckpoint = idx;
        Optimize( );
    }

    void Stream::PopCheckpoint( size_t idx ){
        //Remove a checkpoint from the list of checkpoints.
        //If the checkpoint being removed is the earliest, find the next earliest.
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
        //If 5Kb of empty space exists before the earliest checkpoint, move the occupied memory
        //back so that the empty space is sitting at the end of the array instead.
        if( earliestCheckpoint > (size_t)-3 )
            return;
        if( earliestCheckpoint - offset > 5000 ){
            size_t change = earliestCheckpoint - offset;
            memcpy( buffer+earliestCheckpoint-offset, buffer, bufferend-earliestCheckpoint );
            offset += change;
            reserved += change;
        }
    }

    Stream::Token Stream::TokenEnd( std::vector<std::string> delimiters ){
        size_t i = readPosition;
        std::string delimiter = "empty";
        bool valid = false;
        for( ; i < bufferend; ++i ){
            bool found = true;
            for( unsigned int delimiteri = 0; delimiteri < delimiters.size(); ++delimiteri ){
                delimiter = delimiters[delimiteri];
                if( delimiter == "" ){
                    valid = true;
                    found = true;
                    break;
                }

                //Determine if the next delimiter.length() bytes contain the delimiter
                found = true;
                for( size_t j = 0; j < delimiter.length() && i + j < bufferend; ++j ){
                    if( buffer[i + j - offset] != delimiter[j] ){
                        found = false;
                        break;
                    }
                    if( j+1 == delimiter.length() ){
                        valid = true;
                        found = true;
                        break;
                    }
                }
                if( found )
                    break;
            }
            if( found )
                break;
        }
        //If the previous loop was terminated without setting the valid flag,
        //then return npos.
        Token ret;
        if( !valid ){
            ret.pos = std::string::npos;
            ret.delimiter = "";
            return ret;
        }
        ret.pos = i;
        ret.delimiter = delimiter;
        return ret;
    }



    Stream::Checkpoint::Checkpoint( Stream* s, size_t idx ){
        stream = s;
        index = idx;
        stream->PushCheckpoint( idx );
    }


    Stream::Checkpoint::~Checkpoint(){
        if( stream != nullptr )
            stream->PopCheckpoint( index );
    }


    Error Stream::Checkpoint::Remove(){

        if( stream != nullptr ){
            stream->PopCheckpoint( index );
            stream = nullptr;
        }
        return Error::None;
    }

    size_t Stream::Checkpoint::operator-(Checkpoint other){
        if( other.index > index ){
            return other.index - index;
        }
        return index - other.index;
    }
    Stream::Checkpoint& Stream::Checkpoint::operator++(){
        stream->PopCheckpoint( index );
        index++;
        stream->PushCheckpoint( index );
        return *this;
    }
    Stream::Checkpoint& Stream::Checkpoint::operator++(int){
        stream->PopCheckpoint( index );
        index++;
        stream->PushCheckpoint( index );
        return *this;
    }



    Stream::Stream(){
        buffer = nullptr;
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
        //Return a raw pointer of given size.
        //The data must be committed with a subsequent call to CommitBuffer().
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
        //Any data written to a buffer provided by GetBuffer will only be acknowledged
        //upon calling CommitBuffer().
        if( bufferend + length <= reserved ){
            bufferend += length;
        }
        else
            return Error::OutOfBounds;
        return Error::None;
    }


    Error Stream::PushBuffer( const void* data, size_t length ){
        //Push a buffer full of data onto the end of this buffer.
        void *writeto = GetBuffer( length );
        memcpy( writeto, data, length );
        CommitBuffer( length );
        return Error::None;
    }

    Stream::Checkpoint Stream::End(){
        Checkpoint chk( this, bufferend );
        return chk;
    }


    bool Stream::HasLine(){
        return HasToken( "\n" );
    }


    bool Stream::HasChar(){
        return readPosition < bufferend;
    }

    bool Stream::HasByte(){
        return readPosition < bufferend;
    }


    std::string Stream::PeekLine(){
        return PeekToken( "\n" );
    }


    int Stream::PeekChar(){
        if( readPosition < bufferend )
            return buffer[readPosition - offset];
        return 1000;
    }

    byte Stream::PeekByte(){
        if( readPosition < bufferend )
            return (byte)buffer[readPosition - offset];
        return 0;
    }


    std::string Stream::GetLine(){
        return GetToken( "\n" );
    }

    int Stream::GetChar(){
        int chr = PeekChar();
        if( chr != 1000 )
            readPosition++;
        Optimize( );
        return chr;
    }

    byte Stream::GetByte(){
        byte ret = (byte) GetChar();
        return ret;
    }



    bool Stream::HasData( Checkpoint end ){
        if( end.index - readPosition <= bufferend )
            return true;
        return false;
    }

    void* Stream::PeekData( Checkpoint end ){
        Optimize( );
        if( HasData( end ) ){
            return buffer + readPosition - offset;
        }
        return nullptr;
    }

    void* Stream::GetData( Checkpoint end ){
        //Retrieve the data between the read position and the end checkpoint.
        Optimize( );
        if( HasData( end ) ){
            void* ret = buffer + readPosition - offset;
            LoadCheckpoint( end );
            return ret;
        }
        return nullptr;
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

