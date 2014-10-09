
namespace GlobalMUD{
    void BigNum::Resize( unsigned int size ){
        if( Data.size() < size )
            Data.resize(size);
    }

    void BigNum::Clear(){
        for( unsigned int i = 0; i < Data.size(); ++i ){
            Data[i] = 0;
        }
    }

    BigNum::BigNum(){
        Resize(1);
        Data[0] = 0;
    }

    BigNum::BigNum( long long start ){
        operator=(start);
    }

    BigNum::BigNum( BigNum& start ){
        operator=(start);
    }

    BigNum::~BigNum(){

    }

    BigNum::operator unsigned long long(){
        Resize( sizeof(unsigned long long) );
        long long ret = 0;
        for( unsigned int i = 0; i < sizeof( unsigned long long ); ++i ){
            ret += Data[i] << (i * 8);
        }
        return ret;
    }

    BigNum& BigNum::operator=( BigNum other ){
        Resize( other.Data.size() );
        for( unsigned int i = 0; i < other.Data.size(); ++i ){
            Data[i] = other.Data[i];
        }
        return *this;
    }

    BigNum& BigNum::operator=( unsigned long long other ){
        Resize(sizeof( other ));
        for( int i = 0; other != 0; ++i ){
            Data[i] = other & 0xFF;
            other >>= 8;
        }
        return *this;
    }

    BigNum BigNum::operator>>( BigNum other ){

    }

    BigNum BigNum::operator<<( BigNum other )
    {

    }


    BigNum& operator==( BigNum a, BigNum b ){

    }

    //BigNum& operator==( BigNum a, long long b );
    BigNum& operator!=( BigNum a, BigNum b ){

    }

    BigNum& operator<=( BigNum a, BigNum b ){

    }

    BigNum& operator>=( BigNum a, BigNum b ){

    }

    BigNum& operator<( BigNum a, BigNum b ){

    }

    BigNum& operator>( BigNum a, BigNum b ){

    }


    BigNum& operator*( BigNum a, BigNum b ){

    }

    //BigNum& operator*( BigNum a, long long b );
    BigNum& operator%( BigNum a, BigNum b ){

    }

    //BigNum& operator%( BigNum a, long long b );
    BigNum& operator+( BigNum a, BigNum b ){

    }

    //BigNum& operator+( BigNum a, long long b );
}
