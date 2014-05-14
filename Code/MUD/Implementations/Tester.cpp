#include<cstdlib>

bool Assert( int value ){
    if( value )
        return true;
    #ifdef AssertAbortOnFailure
    exit(0);
    #else
    return false;
    #endif // AssertAbortOnFailure
}
