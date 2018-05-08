#include<iostream>
using namespace std;

/*假设DB的close方法会抛出异常*/
class DB{
public:
    DB(){};
    void close(){throw "error";}
};
/*DB管理类,要负责关闭DB
 * 如果需要对某个函数在运行期间抛出的异常作出响应,
 * 应该提供一个普通函数而不是在析构函数中执行该操作
 * 参见第17行*/
class DBConn{
public:
    DBConn(DB db_):db(db_),closed(false){}
    void close(){              //供用户调用,把可能
                               //抛出异常的函数移出构造函数,给用户处理,
        db.close();            //即不会破坏当前的管理类对象
        closed=true;
    }
    ~DBConn(){
        if(!closed){        //析构函数不要吐出异常,如果调用的函数会抛出异常,
            try{            //应该吞下她们或结束程序,因为抛出异常没人处理.
                db.close();
            }
            catch(...){
                //制作运转记录,记下对close调用的失败
                //...
               // abort();//必要时可以直接终止程序
            }
        }
    }
private:
    DB db;
    bool closed;
};

int main(){
    DB db;
    DBConn dbc(db);
    try{
        dbc.close();
    }catch(const char* ex){
        cout<<string(ex)<<endl;
    }
}
