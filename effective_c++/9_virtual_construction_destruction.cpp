/*在构造函数和析构函数期间不要调用虚函数
 * 由于无法使用虚函数从基类向下调用,在构造期间
 * 可以藉由"令派生类将必要的信息向上传递至基类的构造函数"
 * 来弥补这方面的功能*/
#include<iostream>
using namespace std;

class Transaction{
public:
    explicit Transaction(const string& info);
    void logTransaction(const string& info);
};

Transaction::Transaction(const string& info){
    //...
    logTransaction(info);
}

class BuyTransaction:public Transaction{
public:
    BuyTransaction(char* a):Transaction(string(a)){
    }
private:
    static string a;
};

