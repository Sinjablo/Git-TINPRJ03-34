const int motorForward5 = 4;
const int motorBackward5 = 5;
const int motorForward10 = 6;
const int motorBackward10 = 7;
const int motorForward20 = 8;
const int motorBackward20 = 9;
const int motorForward50 = 10;
const int motorBackward50 = 11;
//int noteArray[9] = {45, 1, 10, 0, 0, 0, 0, 0, 0};
int noteArray[9] = {45, 1, 10, 1, 5, 1, 20, 1, 50};

void setup() {
  pinMode(motorForward5, OUTPUT);
  pinMode(motorBackward5, OUTPUT);
  pinMode(motorForward10, OUTPUT);
  pinMode(motorBackward10, OUTPUT);
  pinMode(motorForward20, OUTPUT);
  pinMode(motorBackward20, OUTPUT);
  pinMode(motorForward50, OUTPUT);
  pinMode(motorBackward50, OUTPUT);

  Serial.begin(9600);

  //motoren resetten
  digitalWrite(motorBackward5, HIGH);
  digitalWrite(motorBackward10, HIGH);
  digitalWrite(motorBackward20, HIGH);
  digitalWrite(motorBackward50, HIGH);
  delay(200);
  digitalWrite(motorBackward5, LOW);
  digitalWrite(motorBackward10, LOW);
  digitalWrite(motorBackward20, LOW);
  digitalWrite(motorBackward50, LOW);


}

void loop() {
  dispenseMoney();
}


void dispenseMoney() {
  for (int i = 2; i < 9; i = i + 2) {
    if (noteArray[i] == 50) {
      dispense50(noteArray[i - 1]);
    } else if (noteArray[i] == 20) {
      dispense20(noteArray[i - 1]);
    } else if (noteArray[i] == 10) {
      dispense10(noteArray[i - 1]);
    } else if (noteArray[i] == 5) {
      dispense5(noteArray[i - 1]);
    } else if (noteArray[i] == 0) {
      break;
    }
    noteArray[i] = 0;
    noteArray[i - 1] = 0;
  }
}

void dispense50(int value) {
  for (value; value != 0; value--) {
    Serial.println("50 word uitgeworpen");
    digitalWrite(motorForward50, HIGH);
    delay(1900);
    digitalWrite(motorForward50, LOW);
    digitalWrite(motorBackward50, HIGH);
    delay(1000);
    digitalWrite(motorBackward50, LOW);
  }
}
void dispense20(int value) {
  for (value; value != 0; value--) {
    Serial.println("20 word uitgeworpen");
    digitalWrite(motorForward20, HIGH);
    delay(2000);
    digitalWrite(motorForward20, LOW);
    digitalWrite(motorBackward20, HIGH);
    delay(1000);
    digitalWrite(motorBackward20, LOW);
  }
}
void dispense10(int value) {
  for (value; value != 0; value--) {
    Serial.println("10 word uitgeworpen");
    digitalWrite(motorForward10, HIGH);
    delay(1900);
    digitalWrite(motorForward10, LOW);
    digitalWrite(motorBackward10, HIGH);
    delay(1000);
    digitalWrite(motorBackward10, LOW); 
  }
}
void dispense5(int value) {
  for (value; value != 0; value--) {
    Serial.println("5 word uitgeworpen");
    digitalWrite(motorForward5, HIGH);
    delay(1850);
    digitalWrite(motorForward5, LOW);
    digitalWrite(motorBackward5, HIGH);
    delay(1000);
    digitalWrite(motorBackward5, LOW);
  }
}
