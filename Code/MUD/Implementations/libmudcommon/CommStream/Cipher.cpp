#include "CommStream/Cipher.hpp"
#include <cstdlib>

namespace MUD{
    //All of the cipher class is currently a NOP for the most part.
    DLL_EXPORT Ciphers::Cipher::Cipher(){

    }

    DLL_EXPORT Ciphers::Cipher::~Cipher(){

    }

    DLL_EXPORT Error Ciphers::Cipher::Encrypt( uint8_t *buffer, size_t size ){
        return Error::None;
    }

    DLL_EXPORT Error Ciphers::Cipher::Decrypt( uint8_t *buffer, size_t size ){
        return Error::None;
    }

    DLL_EXPORT Ciphers::XOR::XOR(){

    }

    DLL_EXPORT Ciphers::XOR::~XOR(){

    }

    DLL_EXPORT Error Ciphers::XOR::Encrypt( uint8_t *buffer, size_t size ){
        for( unsigned int i = 0; i < size; ++i ){
            buffer[i] ^= ( TwisterOut() >> 8 ) & 0xFF;
        }
        return Error::None;
    }

    DLL_EXPORT Error Ciphers::XOR::Decrypt( uint8_t *buffer, size_t size ){
        for( unsigned int i = 0; i < size; ++i ){
            buffer[i] ^= ( TwisterIn() >> 8 ) & 0xFF;
        }
        return Error::None;
    }

    DLL_EXPORT Error Ciphers::XOR::Seed( uint32_t seed1, uint32_t seed2 ){
        TwisterIn.seed(seed1);
        TwisterOut.seed(seed2);
        return Error::None;
    }

    #ifdef RunUnitTests
    DLL_EXPORT bool Ciphers::Cipher::RunTests(){
        //Verify that the basic cipher is a NOP
        TEST("MUD::Ciphers::Cipher");
        Cipher test;
        uint8_t buffer[1000];
        uint8_t mirror[1000];
        for( int i = 0; i < 1000; ++i ){
            buffer[i] = rand();
            mirror[i] = buffer[i];
        }
        test.Encrypt( buffer, 1000 );
        test.Decrypt( buffer, 1000 );
        for( int i = 0; i < 1000; ++i ){
            if( buffer[i] != mirror[i] ){
                FAIL("Unit test failed.");
                return false;
            }
        }
        return true;
    }

    DLL_EXPORT bool Ciphers::XOR::RunTests(){
        //Verify that the XOR cipher returns the same data when run over a given data set twice
        TEST("MUD::Ciphers::XOR");
        XOR test;
        test.Seed( 12345, 12345 );
        uint8_t buffer[1000];
        uint8_t mirror[1000];
        for( int i = 0; i < 1000; ++i ){
            buffer[i] = rand();
            mirror[i] = buffer[i];
        }
        test.Encrypt( buffer, 1000 );
        test.Decrypt( buffer, 1000 );
        for( int i = 0; i < 1000; ++i ){
            if( buffer[i] != mirror[i] ){
                FAIL("Unit test failed.");
                return false;
            }
        }

        test.Seed( 12345, 54321 );
        test.Encrypt( buffer, 1000 );
        test.Decrypt( buffer, 1000 );
        int counter = 1000;
        for( int i = 0; i < 1000; ++i ){
            if( buffer[i] == mirror[i] ){
                counter--;
            }
        }
        if( counter == 0 ){
            FAIL("Unit test failed.");
            return false;
        }
        return true;
    }
    #endif
}
