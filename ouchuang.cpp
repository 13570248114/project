#include<iostream>
#include<string>
using namespace std;
 
int main(){
    string s;
    cin>>s;
    int len = s.size();
    if(len%2)
        len--;
    int half = len/2 -1,cur = len-1;
    while(half!=0){
        if(s[half]!=s[cur]){
            len -= 2;
            half = len/2-1;
            cur = len-1;
        }
        else{
            cur--;
            half--;
        }
    }
    cout<<len<<endl;
    return 0;
}

