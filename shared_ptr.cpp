#include<iostream>
#include<memory>
using namespace std;

int main(){
    int a;
    shared_ptr<int> b(&a);
}
