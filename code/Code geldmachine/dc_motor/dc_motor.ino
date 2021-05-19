

const int motorForward = 5;
const int motorBackward = 4;
int i;


void setup() {
  pinMode(motorForward, OUTPUT);
  pinMode(motorBackward, OUTPUT);
  Serial.begin(9600);

  digitalWrite(motorBackward, HIGH);
  delay(100);
  digitalWrite(motorBackward, LOW);


}

void loop() {
  delay(1000);
  Serial.println("Hoeveel briefjes wil je?");
  while (Serial.available() == 0) {
    // Wait for User to Input Data
  }
  i = Serial.parseInt();
  if (i == 0) {
    Serial.println("Okay, dan niet.");
  } else {
    Serial.print("Er worden nu ");
    Serial.print(i);
    Serial.println(" briefjes uitgeworpen.");
  }

  geldPrinten(i);

}


int geldPrinten(int i){
  while (i != 0) {
    digitalWrite(motorForward, HIGH);
    delay(2000);
    digitalWrite(motorForward, LOW);
    digitalWrite(motorBackward, HIGH);
    delay(800);
    digitalWrite(motorBackward, LOW);
    i--;
  }
}
