#include <arduino.h>

String rekeningNummer;
String uid;
/*----------- I2C library ----------- */
  #include <Wire.h>
  
  /*----------- RFID libraries en setup ----------- */
  #include <SPI.h>
  #include <MFRC522.h>
  
const int RST_PIN = 13; // Reset pin
const int SS_PIN = 21; // Slave select pin
  MFRC522 mfrc522(SS_PIN, RST_PIN);
  
  /*----------- Printer libraries en setup ----------- */
  #include "Adafruit_Thermal.h"
  #include "SoftwareSerial.h"
  //#include "RTClib.h"
  
  #define TX_PIN 14  //RX op printer (blauwe kabel(6))
  #define RX_PIN 1  //TX op printer (groene kabel(5))
  
  SoftwareSerial mySerial(RX_PIN, TX_PIN);
  Adafruit_Thermal printer(&mySerial);
  //RTC_Millis rtc;
  
  /*----------- Keypad library en setup ----------- */
  #include <Keypad.h>
  
  const byte ROWS = 4; 
  const byte COLS = 4;
  byte rowPins[ROWS] = {4, 5, 6, 7}; //begin kant van ster 
  byte colPins[COLS] = {3, 9, 1, 10};
  
  char keyMap [ROWS] [COLS] = { 
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
  };
  
  //Keypad myKeypad = Keypad( makeKeymap(keyMap), rowPins, colPins, ROWS, COLS);
  
  
  /*----------- Variabele ----------- */
  String content = "";
  char key = '\0';
  char pasnummer[16];
  
  boolean boolPrint = false;
  int bedrag = 0;
  
  unsigned long lastTime;
  
  
  /*Het opgenomen bedrag opslaan en de bonprint variabele op true zetten om bon te printen*/
  void receiveEvent() {
    bedrag = Wire.read();
    boolPrint = true;
  }
  
  /*Pasnummer en ingedrukte toets opsturen wanneer er om informatie wordt gevraagd en key resetten om dubbele toetsen te voorkomen*/
  void requestEvent() {
    Wire.write(key);
    Wire.write(pasnummer);  
    key = 0;
  }
  
  void keypadLezen(){

  }
  
void rfidReader(){
	// Look for new cards
	
	// Select one of the cards
	
	if (!mfrc522.PICC_ReadCardSerial()){
		return;
	}

	rekeningNummer = "";
    // Prepare key - all keys are set to FFFFFFFFFFFFh at chip delivery from the factory.
  	MFRC522::MIFARE_Key key;
  	for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;
  
  	MFRC522::StatusCode status;

	byte block = 1;
  	byte len;
  	byte readBuffer[18];

	status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 1, &key, &(mfrc522.uid));
  	if (status != MFRC522::STATUS_OK) {
  	  Serial.print(F("Authentication failed: "));
  	  Serial.println(mfrc522.GetStatusCodeName(status));
  	  return;
  	}

  	status = mfrc522.MIFARE_Read(block, readBuffer, &len);
  	if (status != MFRC522::STATUS_OK) {
  	  Serial.print(F("Reading failed: "));
  	  Serial.println(mfrc522.GetStatusCodeName(status));
  	  return;
  	}
	
  	Serial.print("Rekening Nummer: ");
  	for (uint8_t i = 0; i < 16; i++){
  	  rekeningNummer += (char)readBuffer[i];
  	}

	mfrc522.PICC_HaltA();
  	mfrc522.PCD_StopCrypto1();
	
  	Serial.println(rekeningNummer);
	
  	uid = "";
  	byte letter;
  	for (byte i = 0; i < mfrc522.uid.size; i++){
  	   uid.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
  	   uid.concat(String(mfrc522.uid.uidByte[i], HEX));
  	}
  	uid.toUpperCase();
	
  	Serial.print("UID: ");
  	Serial.println(uid);
	
  	Serial.println(F("\n**End Reading**\n"));
}
  
  void printBon(){
   //De tijd en datum van dit tijdstip bepalen voor later gebruik op de bon
    //rtc.begin(DateTime(F(__DATE__), F(__TIME__)));
    //DateTime now = rtc.now();
  
    printer.println();
    printer.println();
  
  
    
    //De naam van onze bank
    printer.justify('C');
    printer.setSize('L');
    printer.print(F("BOEJIEBANK\n"));  
    printer.justify('L');
    printer.setSize('S');


    printer.justify('C');
    printer.setSize('L');
    //printer.println(F("Tot ziens"));
    printer.println(F("BOEJIEBANK\n"));
    printer.justify('L');
    printer.setSize('S');
    printer.println();
    printer.println();

    /*
   //De datum van het opnemen
    printer.print(F("Datum : "));
    printer.print(now.day(), DEC);
    printer.print('/');
    printer.print(now.month(), DEC);
    printer.print('/');
    printer.println(now.year(), DEC);
  
   //De tijd van het opnemen
    printer.print(F("Tijd : "));
    printer.print(now.hour(), DEC);
    printer.print(':');
    if(now.minute() < 10){
      printer.print(F("0"));
    }
    printer.print(now.minute(), DEC);
    printer.print(':');
    printer.println(now.second(), DEC);
    //printer.println();
    */
   // REKENING NUMMER
    printer.println("Kaart nummer : *********\n");
  
   //Het opgenomen bedrag
    printer.setSize('M');
    printer.print("Opgenomen bedrag: €100 ");
    printer.println(bedrag);
    printer.println();
    
    //Het bedankt bericht
    printer.justify('C');
    printer.setSize('M');
    printer.println(F("Tot ziens"));
    printer.println(F("bij BOEJIEBANK!\n"));
    printer.println();
    printer.println();
  
   //Zet de variabele voor het bonprinten weer op false om de bon niet te blijven printen
    boolPrint = false; 
  }

void setup() {
   //Start de seriële monitor
    Serial.begin(115200);
    
    //Setup voor de I2C verbinding met de ESP
    //Wire.begin(13); 
   // Wire.onRequest(requestEvent);
   // Wire.onReceive(receiveEvent);
    
    //Setup voor de RFID reader
    SPI.begin();
    mfrc522.PCD_Init();
  
   //Setup voor de printer
    mySerial.begin(9600);
    printer.begin();
  
   //Interrupt om deel van de keypad te laten werken voordat er een pas gescanned is
   //attachInterrupt(digitalPinToInterrupt(3), keypadLezen, RISING);

  }

void loop() {

   //Als boolPrint aan staat moet er een bon geprint worden
   if(mfrc522.PICC_IsNewCardPresent()){
		
		rfidReader();
		Serial.println("--------------------------------------------------------------loc1");
		delay(200);
	}
    
}