#include<cstdlib>
#include "Tester.hpp"

bool Assert( int value ){
    if( value )
        return true;
    #ifdef AssertAbortOnFailure
    exit(0);
    #else
    return false;
    #endif // AssertAbortOnFailure
}
