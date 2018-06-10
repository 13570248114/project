#include<iostream>
#include<boost/shared_ptr.hpp>
using namespace std;

void mydelete(int* a8){
    cout<<"delete a8"<<endl;
    delete[] a8;
}

int main()
{
    int* a8 = new int[8];
    shared_ptr<int> share(a8,mydelete);
}
