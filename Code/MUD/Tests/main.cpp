#include<cstdio>
#include "Tester.hpp"
#include "CommStream/CommStream.hpp"
#include "CommStream/Cipher.hpp"
#include "Global/Thread.hpp"
#include "HTTPd/HTTPd.hpp"
#include "Global/Error.hpp"

#include <random>
#include <cstdio>
#include<functional>
#include "Telnet/Telnet.hpp"

int testfunc(int a, std::string b){
    printf("%s\t%d\n", b.c_str(), a);
}
int test( std::function<int(int)> f ){
    return f( 6 );
}

int main(){
    test( std::bind(testfunc, std::placeholders::_1, "Test" ) );
    #ifdef _WIN32
    WSADATA globalWSAData;
    WSAStartup( MAKEWORD(2, 2), &globalWSAData );
    #endif

    Test<GlobalMUD::Thread>();
    Test<GlobalMUD::Ciphers::Cipher>();
    Test<GlobalMUD::Ciphers::XOR>();
    Test<GlobalMUD::CommStream>();
    Test<GlobalMUD::HTTPd>();
}
