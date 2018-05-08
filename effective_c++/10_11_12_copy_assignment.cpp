#include<iostream>
#include<algorithm>
using namespace std;

class myint{
public:
    int a;
    char *bitt;
public:
    void myswap(myint& rhs){
        swap(a,rhs.a);
        swap(bitt,rhs.bitt);
    }
    myint& operator=(const myint& rhs){
        myint temp(rhs);
        myswap(temp);
        return *this;
    }
    myint(int a_):a(a_),bitt(nullptr){}
    myint():a(0),bitt(nullptr){}
};

class myintint:public myint{
private:
    int b;
public:
    myintint& operator=(const myintint& rhs){
        myint::operator=(rhs);   //记得要调用子类的赋值函数
        b=rhs.b;
        return *this;
    }
    myintint(int c_,myint a_):b(c_),myint(a_){}
    myintint(){}
};

int main(){
    myint a(1);
    myint b;
    (b=a)=1;
    cout<<b.a<<endl;
    myintint c(10,a);
    myintint d;
    d = c;
    cout<<d.a<<endl;
}
