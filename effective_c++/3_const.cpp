#include<iostream>
#include<string>
using namespace std;

class testConst{
private:
    string buf;
    int a;
public:
    testConst(string buff,int aa):buf(buff),a(aa){}
    const char& operator[](std::size_t position) const{
        cout<<a<<endl;
        return buf[position];
    }

    char& operator[](std::size_t position){
        a++;
        return const_cast<char&>(static_cast<const testConst&>(*this)[position]);
        
    }
};

int main(){
    testConst s("abc",2);
    const testConst ss(s);
    cout<<s[2]<<endl;
    cout<<ss[2]<<endl;
}
