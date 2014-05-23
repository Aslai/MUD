#include<cstdio>
#include "Tester.hpp"
#include "CommStream/CommStream.hpp"
#include "CommStream/Cipher.hpp"
#include "Global/Thread.hpp"

#include <random>
#include <cstdio>

int main(){
    #ifdef _WIN32
    WSADATA globalWSAData;
    WSAStartup( MAKEWORD(2, 2), &globalWSAData );
    #endif
    Test<GlobalMUD::Thread>();
    Test<GlobalMUD::Ciphers::Cipher>();
    Test<GlobalMUD::Ciphers::XOR>();
    Test<GlobalMUD::CommStream>();
}
