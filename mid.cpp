#include<vector>
#include<iostream>
using namespace std;

class Solution {
public:
    int minNumberInRotateArray(vector<int> rotateArray) {
        if(rotateArray.size()==0)
            return 0;
        int left = 0;
        int right = rotateArray.size()-1;
        int mid;
        while(left<=right){
            mid = left+(right-left)/2;
            if(rotateArray[mid]<=rotateArray[left]&&rotateArray[mid]<=rotateArray[right]){
                return rotateArray[mid];
            }
            else if(rotateArray[mid]>rotateArray[right]){
                left = mid+1;
            }
            else{
                right = mid-1;
            }
        }
        return 0;
    }
};

int main()
{
    vector<int> a = {6501,6828,6963,7036,7422,7674,8146,8468,8704,8717,9170,9359,9719,9895,9896,9913,9962,154,293,334,492,1323,1479,1539,1727,1870,1943,2383,2392,2996,3282,3812,3903,4465,4605,4665,4772,4828,5142,5437,5448,5668,5706,5725,6300,6335};
    Solution s;
    int k= s.minNumberInRotateArray(a);
    cout<<k<<endl;
}
