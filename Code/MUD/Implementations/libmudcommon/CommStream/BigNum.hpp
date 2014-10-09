#include <vector>

namespace GlobalMUD{
    class BigNum{
        std::vector<unsigned char> Data;
        void Resize( unsigned int size );
        void Clear();
    public:
        BigNum();
        BigNum( long long start );
        BigNum( BigNum& start );
        ~BigNum();
        operator unsigned long long();
        BigNum& operator=( BigNum other );
        BigNum& operator=( unsigned long long other );
        BigNum operator>>( BigNum other );
        BigNum operator<<( BigNum other );

    };
    bool operator==( BigNum a, BigNum b );
    //BigNum& operator==( BigNum a, long long b );
    bool operator!=( BigNum a, BigNum b );
    bool operator<=( BigNum a, BigNum b );
    bool operator>=( BigNum a, BigNum b );
    bool operator<( BigNum a, BigNum b );
    bool operator>( BigNum a, BigNum b );

    BigNum& operator*( BigNum a, BigNum b );
    //BigNum& operator*( BigNum a, long long b );
    BigNum& operator%( BigNum a, BigNum b );
    //BigNum& operator%( BigNum a, long long b );
    BigNum& operator+( BigNum a, BigNum b );
    //BigNum& operator+( BigNum a, long long b );
}
