#include<algorithm>
#include<iostream>
using namespace std;

class WidgetImpl
{
public:
    WidgetImpl(int x,int y,int z):a(x),b(y),c(z){}
    //...
    int a,b,c;
};

class Widget
{
public:
    Widget(WidgetImpl* w):pIml(w){}
    Widget(const Widget& rhs);
    Widget& operator=(const Widget& rhs)
    { *pIml=*(rhs.pIml); return *this;}
    
    void swap(Widget& other)
    {
        using std::swap;
        swap(pIml,other.pIml);
    }

    WidgetImpl* pIml;
};

namespace std
{
    template<>
    void swap<Widget>(Widget& a,Widget& b)
    { a.swap(b); }
}

int main()
{
    WidgetImpl w1(1,2,3);
    WidgetImpl w2(4,5,6);
    Widget wg1(&w1);
    Widget wg2(&w2);
    swap(wg1,wg2);
    cout<<w1.a<<" "<<w2.a<<endl;
    cout<<wg1.pIml->a<<" "<<wg2.pIml->a<<endl;
}


