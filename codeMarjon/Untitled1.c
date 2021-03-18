#include <stdio.h>
#include <stdlib.h>

int main(){
int a = 1111;
int b = 2222;
int c = 3333;

int pinCode;

for(int fout = 0; fout <= 4; fout++){
    printf("Voer uw pincode in: \n");
    scanf ("%d", &pinCode);
    if(pinCode == a || pinCode == b || pinCode==c){
        printf("Goed\n");
        break;
    }
    else
        printf("Fout!\n");
        fout++;
    }
return 0;
}
