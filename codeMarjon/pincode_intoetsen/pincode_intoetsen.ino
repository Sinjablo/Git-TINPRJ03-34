#include <arduinio.h>
#include <stdlib.h>
 
int a = 1111;
int pogingen = 3;
int pinCode;

void setup(){
    Serial.begin (9600);
    }

void loop() {
  // put your main code here, to run repeatedly:
  for(int fout = 0; fout != 2; fout++){

    if(pinCode == a){
      Serial.println("Goed");
      break;
    }else{
      Serial.println("Fout!");
      fout++; 
    }
  }
}
