#include<vector>
#include<iostream>
using namespace std;

class Solution {
public:
    bool Find(int target, vector<vector<int> > array) {
        if(array.size()==0)
            return false;
        int row = array.size();
        int col = array[0].size();
        int r = 0;
        int c = col-1;
        while(r<row&&c>=0&&target!=array[r][c])
        {
            if(array[r][c]>target)
                c--;
            if(array[r][c]<target)
                r++;
        }
        return array[r][c]==target;
    }
};

int main()
{
    vector<vector<int>> s{{1,2},{3,4}};
    Solution ss;
    ss.Find(1,s);
}
