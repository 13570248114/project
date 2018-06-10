/*使用多态时基类一定要声明虚析构函数*/
#include<iostream>
using namespace std;

class base{
public:
    virtual void buy(){cout<<0<<endl;}
    virtual ~base(){cout<<"delete 0"<<endl;}
};

class derived:public base{
public:
    void buy(){cout<<1<<endl;}
    ~derived(){cout<<"delete 1"<<endl;}
};

int main(){
    base* a = new base();
    a->buy();
    base* b = new derived();
    b->buy();
}

