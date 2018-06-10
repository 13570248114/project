#include<iostream>
#include<vector>
using namespace std;
inline long long max(long long a,long long b){
    return a>b?a:b;
}

inline long long min(long long a,long long b){
    return a<b?a:b;
}

int kMax(vector<int>& a,int n,int k,int d){
    if(a.size()!=n+1)
        return -1;
    if(k<=0)
        return 0;
    long pos[51][11];
    long neg[51][11];
    for(int i=1;i<=n;i++){
        pos[i][1]=a[i];
        neg[i][1]=a[i];
    }
    
    long maxval,minval;
    for(int kk=2;kk<=k;kk++){
        for(int i=kk;i<=n;i++){
            maxval=pos[i-1][kk-1]*a[i];
            minval=neg[i-1][kk-1]*a[i];
            for(int m = max(k-1,i-d);m<i;m++){
                long tempMax = max(pos[m][kk-1]*a[i],neg[m][kk-1]*a[i]);
                maxval = max(maxval,tempMax);
                long tempMin = min(pos[m][kk-1]*a[i],neg[m][kk-1]*a[i]);
                minval = min(minval,tempMin);
            }
            pos[i][kk]=maxval;
            neg[i][kk]=minval;
        }
    }
    long res = pos[k][k];
    for(int i=k+1;i<=n;i++){
        res = res>pos[i][k]?res:pos[i][k];
    }
    return res;
}

int main(){
    int n,k,d,temp;
    cin>>n;
    vector<int> a(n+1);
    for(int i=1;i<=n;i++){
       cin>>a[i];
    }
    cin>>k>>d;
    cout<<kMax(a,n,k,d)<<endl;
    return 0;
}
