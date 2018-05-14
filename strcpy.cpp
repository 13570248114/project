#include<iostream>
#include<assert.h>
#include<string.h>
#include<cstdio>
using namespace std;

char* copy(const char* src, char* dis);

int main()
{
   char a[]={'a','b','c','\0'};
   char b[2];
   copy(a,a+1);
   printf("%s\n",a);
}

char* copy(const char* src, char* dis)
{
    assert(src!=NULL&&dis!=NULL);
    int len = strlen(src);
    if(dis-src>0)
    {
        *(dis+len)='\0';
        len--;
        while(len>=0)
        {
            *(dis+len)=*(src+len);
            len--;
        }
    }
    else
    {
        int i = 0;
        while(i<len)
        {
            *(dis+i)=*(src+i);
            i++;
        }
        *(dis+i)='\0';
    }
    return dis;
}
