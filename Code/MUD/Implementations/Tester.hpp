#include <cstdio>

bool Assert(int value);
template<class T>
class Test{
public:
    Test(){
        if( T::RunTests() ){
            printf("Test Succeeded.\n\n");
        }
        else {
            printf( "Test failed.\n\n" );
        }
    }
};
