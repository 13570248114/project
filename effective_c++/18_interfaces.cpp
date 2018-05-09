#include<iostream>
using namespace std;

/*为接口输入导入新类型
 * 假如要输入一个日期*/

class Month{
public:
    static Month Jan(){return Month(1);}
    static Month Feb(){return Month(2);}
    //...
    static Month Dec(){return Month(3);}
    //...
private:
    explicit Month(int m):month_(m){}
    int month_;
};


