#ifndef MUD_COMMSTREAM_CIPHER_HPP
#define MUD_COMMSTREAM_CIPHER_HPP

#include "libmudcommon/dllinfo.hpp"

#include <string>
#include <random>
#include "libmudcommon/Error/Error.hpp"


namespace MUD{
    namespace Ciphers{
        class Cipher{
        public:
            Cipher();
            virtual ~Cipher();
            virtual Error Encrypt( uint8_t *buffer, size_t size );
            virtual Error Decrypt( uint8_t *buffer, size_t size );
            #ifdef RunUnitTests
            static bool RunTests();
            #endif
        };
        class XOR : public Cipher{
            std::mt19937 TwisterIn;
            std::mt19937 TwisterOut;

            public:
            XOR();
            ~XOR();
            Error Encrypt( uint8_t *buffer, size_t size );
            Error Decrypt( uint8_t *buffer, size_t size );
            Error Seed( uint32_t seed1, uint32_t seed2 );
            #ifdef RunUnitTests
            static bool RunTests();
            #endif
        };
    }
}

#endif
