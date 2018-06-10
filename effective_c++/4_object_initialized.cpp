/* 其实这是单例模式的一种,比如下面两个类存在于不同两个源文件时,
 * 并且其中一个依赖于另外一个的一个实例,因为不同源文件无法保证
 * non-local static对象初始化的先后顺序,应此用local static对象
 * 替换之,能够保证其被初始化*/
#include<iostream>
using namespace std;

class FileStream{
public:
    size_t numDisks() const{return 0;}
};

FileStream& tfs(){
    static FileStream fs;
    return fs;
}

class Directory{
public:
    Directory(){
        size_t disk = tfs().numDisks();
        cout<<disk<<endl;
    }
};

Directory& tempDir(){
    static Directory td;
    return td;
}

int main(){
    Directory d;
}


