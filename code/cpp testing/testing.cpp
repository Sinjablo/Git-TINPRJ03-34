#include <stdio.h>
#include <string>
//#include <iostream>


int main(){

    int money = 80;
    int x = money/50;
    float y = 5/2;
    if(x % 1 != 0){
        printf("not 0\n");
    }else{
        printf("yes 0\n");  // 
    }
    printf("x = %i\n", x);
    printf("float y = %f\n", y);

}