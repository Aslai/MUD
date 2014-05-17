namespace GlobalMUD{
    template<class type>
    class RefCounter{
        type** myptr;
        unsigned int *count;

        void countup(){
            (*count) ++;
        }
        void countdown(){
            (*count)--;
            if( (*count) == 0 ){
                delete count;
                delete *myptr;
                delete myptr;
            }
        }

        public:
        RefCounter(type *t){
            myptr = new type*;
            count = new unsigned int;
            *myptr = t;
            (*count) = 1;
        }
        RefCounter() : RefCounter( new type() ) {

        }
        RefCounter( const RefCounter<type> &t ){
            myptr = t.myptr;
            count = t.count;
            countup();
        }

        ~RefCounter(){
            countdown();
        }
        type*& get(){
            return *myptr;
        }
        type*& operator->(){
            return *myptr;
        }
        type& operator*(){
            return **myptr;
        }
        type& operator[](int idx){
            return (*myptr)[idx];
        }
        /*RefCounter<type>& operator=(RefCounter<type> & t){
            countdown();
            myptr = t.myptr;
            count = t.count;
            countup();
            return *this;
        }*/
        RefCounter<type>& operator=(RefCounter<type> t){
            countdown();
            myptr = t.myptr;
            count = t.count;
            countup();
            return *this;
        }


    };
}
