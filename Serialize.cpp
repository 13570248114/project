#include<iostream>
#include<string>
#include<cstdio>
using namespace std;



struct TreeNode {
    int val;
    struct TreeNode *left;
    struct TreeNode *right;
    TreeNode(int x) :
            val(x), left(NULL), right(NULL) {
    }
};

class Solution {
public:
    typedef char* charPtr;
    char* Serialize(TreeNode *root) {    
        if(root==nullptr)
            return nullptr;
        else{
            return const_cast<char*>(pre(root).c_str());
        }
    }
    TreeNode* Deserialize(char *str) {
        if(str==nullptr)
            return nullptr;
        return DeserializeCore(str);
    }
private:
    string pre(TreeNode *root){
        if(root==nullptr)
            return "#";
        else{
            return to_string(root->val)+"!"+pre(root->left)+pre(root->right);
        }
    }
    
    TreeNode* DeserializeCore(charPtr& ptr){
        if(*ptr=='#'){
            ptr++;
            return nullptr;
        }else{
            int num = 0;
            while(*ptr!='\0'&&*ptr!='!'){
                num = num*10+*ptr-'0';
                ++ptr;
            }
            TreeNode* root = new TreeNode(num);
            if(*ptr=='\0')
                return root;
            ++ptr;
            root->left=DeserializeCore(ptr);
            root->right=DeserializeCore(ptr);
            return root;
        }
    }
};

int main(){
    TreeNode* n1=new TreeNode(0);
    TreeNode* n2=new TreeNode(1);
    TreeNode* n3=new TreeNode(2);
    n1->left = n2;
    n1->right=n3;
}
