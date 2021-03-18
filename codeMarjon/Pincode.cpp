#include <arduinio.h>
#include <stdlib.h>S
 
int a = 1111;
int pogingen = 3;
int pinCode;

void setup(){
    Serial.begin (9500);
    }

void loop() {
  // put your main code here, to run repeatedly:
	for(int i = 0; i != 2; i++){

		if(pincode == a){
			Serial.println("Goed");
			break;
		}else{
			Serial.println("Fout!")
			int fout++;	
		}
	}
}