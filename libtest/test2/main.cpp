#include "myAPI.h"  
#include <iostream>  
#include "cheng.h"
  
int main(){  
    std::cout << "1 + 1 = " << ADD(1, 1) << std::endl;  
    std::cout << "1 - 1 = " << MINUS(1, 1) << std::endl;  
    std::cout << cheng(1,2)<<std::endl;
    return 0;  
}  
