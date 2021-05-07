

const int motorForward = 5;
const int motorBackward = 4;


void setup() {
  pinMode(motorForward, OUTPUT);
  pinMode(motorBackward, OUTPUT);
  Serial.begin(9600);

  digitalWrite(motorBackward, HIGH);
  delay(100);
  digitalWrite(motorBackward, LOW);


}

void loop() {
  Serial.println("Hoeveel briefjes wil je?");
  while (Serial.available() == 0) {
    // Wait for User to Input Data
  }
  int i = Serial.parseInt();
  Serial.print("Er worden nu ");
  Serial.print(i);
  Serial.println(" briefjes uitgeworpen.");

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
