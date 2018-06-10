#include <iostream>
using namespace std;

class constExx{
public:
    static const int five;
    enum {six = 6}; //取const变量的地址合法,取enum的地址不合法,其比较像define
    int num[six];
};

const int constExx::five = 5;

int main(){
    cout<<constExx::five<<endl;
}
