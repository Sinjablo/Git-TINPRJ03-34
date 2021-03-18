//Zeno Modiff
//Keypad Serial Monitor
#include <Keypad.h>

const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns

//define the cymbols on the buttons of the keypads
char hexaKeys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte rowPins[ROWS] = {9, 8, 7, 6}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {5, 4, 3, 2}; //connect to the column pinouts of the keypad

//initialize an instance of class NewKeypad
Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 

const String pincode = "1111";
String input_pincode;

void setup(){
  Serial.begin(9600);
  input_pincode.reserve(5);
  Serial.println ("Voer pincode in");
}
  
void loop(){
  for (int fout = 0; fout <= 4; fout++){
   char key = customKeypad.getKey();
   if (key) {
    Serial.print(key);
    
   if (key == '*'){
      input_pincode == "";
      }else if (key == '#') {
        if (pincode == input_pincode){
          Serial.println("");
          Serial.println("Goed!");
          break;
                     
          }else {
            Serial.println("");
            Serial.println("Fout! probeer opnieuw");
            fout++;
          }
          input_pincode ="";
      }else {
        input_pincode += key;
    }
}
}
}
