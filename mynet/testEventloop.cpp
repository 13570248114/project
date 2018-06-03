#include "EventLoop.h"
#include <iostream>
using namespace std;

void print()
{
    cout<<"hello"<<endl;
}

int main()
{
    EventLoop loop;
    loop.runAfter(Nanosecond(10000000000),print);
    loop.loop();
    return 0;
}
