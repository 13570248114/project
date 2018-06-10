#include<iostream>
using namespace std;

/*通过私有拷贝构造函数和赋值构造函数来禁止拷贝
 * 同理,通过私有构造函数可以实现单例模式*/
class noncopy{
private:
    int a;
    noncopy(const noncopy&);
    noncopy& operator=(const noncopy&);
};

/*或者通过继承一个不能拷贝和复制的基类*/
class noncopyBase{
protected:
    noncopyBase(){}                        //允许子类对象对其构造和析构
    ~noncopyBase(){}
private:
    noncopyBase(const noncopyBase&);
    noncopyBase& operator=(const noncopyBase&);
};

class noncopyDerived:public noncopyBase{
    int a;
};

int main(){
    noncopy n;
    noncopy n1(n);
    noncopyDerived n2;
    noncopyDerived n3(n2);
}

