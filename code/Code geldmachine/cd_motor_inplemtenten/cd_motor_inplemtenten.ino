

const int motorForward5 = 4;
const int motorBackward5 = 5;
const int motorForward10 = 6;
const int motorBackward10 = 7;
const int motorForward20 = 8;
const int motorBackward20 = 9;
const int motorForward50 = 10;
const int motorBackward50 = 11;
int i;
int noteArray[9] = {45, 2, 20, 0, 5, 0, 0, 0, 0};
String start = "hey";

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
  delay(100);
  digitalWrite(motorBackward5, LOW);
  digitalWrite(motorBackward10, LOW);
  digitalWrite(motorBackward20, LOW);
  digitalWrite(motorBackward50, LOW);


}

void loop() {
  dispenseMoney();
}


int dispenseMoney() {

  for (i = 2; i < 9; i = i + 2) {
    if (noteArray[i] == 50) {
      dispense50(noteArray[i - 1]);
      Serial.println("50");
    } else if (noteArray[i] == 20) {
      dispense20(noteArray[i - 1]);
      Serial.println("20");
    } else if (noteArray[i] == 10) {
      dispense10(noteArray[i - 1]);
      Serial.println("10");
    } else if (noteArray[i] == 5) {
      dispense5(noteArray[i - 1]);
      Serial.println("5");
    } else if (noteArray[i] == 0) {
      break;
    }
    Serial.println(i);
    noteArray[i] = 0;
    noteArray[i - 1] = 0;
  }
}

int dispense50(int value) {
  for (value; value != 0; value--) {
    digitalWrite(motorForward50, HIGH);
    delay(1300);
    digitalWrite(motorForward50, LOW);
    digitalWrite(motorBackward50, HIGH);
    delay(800);
    digitalWrite(motorBackward50, LOW);
    Serial.println("50 word uitgeworpen");
  }
}
int dispense20(int value) {
  for (value; value != 0; value--) {
    digitalWrite(motorForward20, HIGH);
    delay(1300);
    digitalWrite(motorForward20, LOW);
    digitalWrite(motorBackward20, HIGH);
    delay(800);
    digitalWrite(motorBackward20, LOW);
    Serial.println("20 word uitgeworpen");
  }
}
int dispense10(int value) {
  for (value; value != 0; value--) {
    digitalWrite(motorForward10, HIGH);
    delay(1300);
    digitalWrite(motorForward10, LOW);
    digitalWrite(motorBackward10, HIGH);
    delay(800);
    digitalWrite(motorBackward10, LOW);
    Serial.println("10 word uitgeworpen");
  }
}
int dispense5(int value) {
  for (value; value != 0; value--) {
    digitalWrite(motorForward5, HIGH);
    delay(1300);
    digitalWrite(motorForward5, LOW);
    digitalWrite(motorBackward5, HIGH);
    delay(800);
    digitalWrite(motorBackward5, LOW);
    Serial.println("5 word uitgeworpen");
  }
}
