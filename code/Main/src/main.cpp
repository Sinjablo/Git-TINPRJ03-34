#include <arduino.h>
#include <stdio.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <FS.h>
#include <Wire.h>
#include <iostream>
#include <string>
#include <keypad.h>
#include <HTTPClient.h>
#include <ESP32WiFi.h> 
#include <SPI.h>
#include <MFRC522.h>
#include <iterator>
#include <map>
#include "Adafruit_Thermal.h"	//printer
#include "SoftwareSerial.h"		//printer


using namespace std; 
// Replace with your network credentials
//const char *ssid = "ASUS1424";
//const char *password = "MaJaNe14245.";

const char *ssid = "Tesla IoT";
const char *password = "fsL6HgjN";

//const char *ssid = "LaptopieVanSander";
//const char *password = "KrijgsheerSander";

//const char *ssid = "lenovolaptop";
//const char *password = "jarno123";

//const char *ssid = "VielvoyeResidence24GHz";
//const char *password = "Oli/5iN-dR=88#VRGHZ#24";

//Access point credentials

const char* host = "http://145.24.222.170"; //IPv4 adress hosting laptop/server
String get_host = "http://145.24.222.170"; //same as above

String userPasscode = "7777";
String key = "de3w2jbn7eif1nw9e";

//--------- Setup keypad----
const byte ROWS = 4; 
const byte COLS = 4; 

// char hexaKeys[ROWS][COLS] = {
//   {'1', '2', '3', 'A'},
//   {'4', '5', '6', 'B'},
//   {'7', '8', '9', 'C'},
//   {'*', '0', '#', 'D'}
// };
char hexaKeys[ROWS][COLS] = {
  {'1', '4', '*', '7'},
  {'2', '5', '0', '8'},
  {'3', '6', '#', '9'},
  {'A', 'B', 'D', 'C'}
};
//byte rowPins[ROWS] = {23, 22, 3, 21}; 
//byte colPins[COLS] = {19, 18, 16, 17};
// byte colPins[COLS] = {16, 4, 2, 0};
// byte rowPins[ROWS] = {22, 3, 5, 17}; 
byte colPins[COLS] = {16, 4, 13, 0};
byte rowPins[ROWS] = {22, 3, 5, 17}; 
Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 
//----------End keypad setUp

//----------Setup RFID reader-------
const int RST_PIN = 13; // Reset pin
const int SS_PIN = 21; // Slave select pin
 
MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance
//-------------end rfid setup

//-------------setup printer----

#define TX_PIN 14  //RX op printer (blauwe kabel(6))
#define RX_PIN 2  //TX op printer (groene kabel(5))
  
SoftwareSerial mySerial(RX_PIN, TX_PIN);
Adafruit_Thermal printer(&mySerial);
//end printer setup

//dummy variables:
String dummyPasscode = "7777";
String dummyTempPasscode;
String dummyRekeningnummer;
String xa = "";
String x;
char g;
const int tempBtn = 4;
String tester = "120205";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);


//Variables
int page = 1;
int withdrawStep = 0;
String rekeningNummer;
String uid;
String passcode;
String tempPasscode;
int endSession;
long sessionTime = 0;
String balance;
bool timerRunning = false;
int pincodeTimeOut = 10000;
int generalTimeOut = 30000;
int noteArray[9];	// array with amount of bills, and the value of what the user wants to withdraw. 0 = total value, 1 = amount of bills of value thats on the next place.
//arrays with the amount and value of bills which the user can choose from to withdraw
int customNoteArray01[9];	
int customNoteArray02[9];
int customNoteArray03[9];
int billsAvailable[4];	// 0 = 5, 1 = 10, 2 = 20, 3 = 50
String billCombinationSelection;
boolean wrongInput = false;
String currency = "&euro;";
String billCombinationString[3];	// a string array containing the combination of bills for the user to choose from
int CustomBillCombinationReturned = 0;
String CustomWithdrawOption;
boolean billsChecked = false;
String leftTabs[4] = {
		"1| ",
		"4| ",
		"7| ",
		"*| "
	};

//Motoren geld dispenser
const int motorForward5 = 1;
const int motorBackward5 = 12;
const int motorForward10 = 15;
const int motorBackward10 = 25;
const int motorForward20 = 26;
const int motorBackward20 = 27;
const int motorForward50 = 32;
const int motorBackward50 = 33;

// variables to return to the GUI website
char navigationKey;
bool abortCheck = false;
bool rfidCheck = false;
char loginCommand = '0';
String lastName = "Vuijk";
String passcodeLenght;
String customAmount = "";

std::map<String, String> billCombinations{	
	{"11", "110000"},
	{"14", "205000"},
	{"17", "000000"},
	{"1*", "000000"},
	{"31", "120110"},
	{"34", "120205"},
	{"37", "310000"},
	{"3*", "605000"},
	{"71", "150120"},
	{"74", "150210"},
	{"77", "320110"},
	{"7*", "710000"}
};

std::map<int, int> billsAvailableIndex{	
	{5, 0},
	{10, 1},
	{20, 2},
	{50, 3}
};

int verifieer_pincode(String passcode, String accountNumber){
    
    //WiFiClient client = server.available();
		
    HTTPClient http;
    String url = get_host+"/verificatie.php?"+"sltl="+key+"&mgrkn="+accountNumber+"&mgpc="+passcode;
    Serial.println(url);
    
    http.begin(url);
    
    //GET method
    int httpCode = http.GET();
    String payload = http.getString();
	Serial.print("payload: ");
    Serial.println(payload);
    http.end(); 
	int intPayload = atoi(((String)payload).c_str());
	return intPayload;
  
}

String getBalans(){
    
    //WiFiClient client = server.available();
    HTTPClient http;
	Serial.print("pincode: ");
	Serial.println(passcode);
    String url = get_host+"/balansS.php?"+"sltl="+key+"&mgrkn="+rekeningNummer+"&mgpc="+passcode;
    Serial.println(url);
    
    http.begin(url);
    
    //GET method
    int httpCode = http.GET();
    String balance = http.getString();
    Serial.println(balance);
    http.end(); 
	return balance;
  
}

String geldOpnemen(){
    
    //WiFiClient client = server.available();
    HTTPClient http;
	Serial.print("pincode: ");
	Serial.println(passcode);
    String url = get_host+"/opnemen.php?"+"sltl="+key+"&mgrkn="+rekeningNummer+"&mgpc="+passcode+"&bdg="+noteArray[0];
    Serial.println(url);
    
    http.begin(url);
    
    //GET method
    int httpCode = http.GET();
    String balance = http.getString();
    Serial.println(balance);
    http.end(); 
	return balance;
  
}

String uitStroom(String noteWorth, String noteAmount, String medewerker, String medewerkerWachtwoord){
	HTTPClient http;
	String url = get_host+"/uitstroom.php?"+"sltl="+key+"&brfj="+noteWorth+"&hvbrfj="+noteAmount+"&mdw="+medewerker+"&mdwww="+medewerkerWachtwoord;
	Serial.print(url);

	http.begin(url);
    
    //GET method
    int httpCode = http.GET();
    String nrOfNotes = http.getString();
    Serial.println(nrOfNotes);
    http.end(); 
	return nrOfNotes;
}

int aantalBriefjes(String briefWaarde, String medewerker, String medewerkerWachtwoord){
	HTTPClient http;
	Serial.print("pincode: ");
	Serial.println(passcode);
    String url = get_host+"/briefjes.php?"+"sltl="+key+"&brfj="+briefWaarde+"&mdw="+medewerker+"&mdwww="+medewerkerWachtwoord;
    Serial.println(url);
    
    http.begin(url);
    
    //GET method
    int httpCode = http.GET();
    String briefAantal = http.getString();
    Serial.println(briefAantal);
    http.end(); 
	int briefAantalInt = atoi(((String)briefAantal).c_str());
	return briefAantalInt;
}

#pragma region 	// String return functions for the webserver to switch pages

String getAbortCheck(){
	bool tempAbortCheck = abortCheck;
	abortCheck = false;
	if(tempAbortCheck == true){
		page = 1;
		passcode = "";
		rekeningNummer = "";
		uid = "";
		balance = "";
		timerRunning = false;
		withdrawStep = 0;
		customAmount = "";
		Serial.println("Abortus has been commited");
	}
	return String(tempAbortCheck);
}
String getNavigation(){
	
	int tempNavigationKey;
	switch (navigationKey)
	{
	case '1':
		tempNavigationKey = 1;
		break;
	case '4':
		tempNavigationKey = 4;
		break;
	case '7':
		tempNavigationKey = 7;
		break;
	case '*':
		tempNavigationKey = 10;
		break;
	case 'A':
		tempNavigationKey = 11;
		break;
	case 'B':
		tempNavigationKey = 12;
		break;
	case 'C':
		tempNavigationKey = 13;
		break;
    default:
		tempNavigationKey = 0;
		break;
	}
	navigationKey = '0';

	return String(tempNavigationKey);
}
String getRfidCheck(){
	bool tempRfidCheck = rfidCheck;
	rfidCheck = false;
	return String(tempRfidCheck);
	
}
String getLoginCommand(){
	char tempPasscodeCheck = loginCommand;
	loginCommand = '0';
	return String(tempPasscodeCheck);
}
String getPasscodeLenght(){
	return String(passcode.length());
}
String getAccountNumber(){
	return String(rekeningNummer);
}
String getBalance(){
	return String(getBalans());
}
String getCustomAmount(){
	return String(customAmount);
}

String getMenuTest(){
	String returnString;
	switch (CustomBillCombinationReturned){
		case 0:
			returnString = billCombinationString[0];
			CustomBillCombinationReturned++;
			break;
		case 1:
			returnString = billCombinationString[1];
			CustomBillCombinationReturned++;
			break;
		case 2:
			returnString = billCombinationString[2];
			CustomBillCombinationReturned = 0;
			break;
	}
	if(CustomWithdrawOption != ""){
		Serial.println("customWithdrawOption in get menu");
		returnString = CustomWithdrawOption;
		CustomWithdrawOption = "";
	}
	return String(returnString);
}

#pragma endregion


void setup(){	// void setup
	// Serial port for debugging purposes
	Serial.begin(115200);

	mySerial.begin(9600);
    printer.begin();

	pinMode(tempBtn, OUTPUT);

	//motoren
	pinMode(motorForward5, OUTPUT);
	pinMode(motorBackward5, OUTPUT);
  	pinMode(motorForward10, OUTPUT);
  	pinMode(motorBackward10, OUTPUT);
  	pinMode(motorForward20, OUTPUT);
  	pinMode(motorBackward20, OUTPUT);
  	pinMode(motorForward50, OUTPUT);
  	pinMode(motorBackward50, OUTPUT);

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

	// Initialize SPIFFS
	if (!SPIFFS.begin())
	{
		Serial.println("An Error has occurred while mounting SPIFFS");
		return;
	}

	// Connect to Wi-Fi
	WiFi.begin(ssid, password);
	while (WiFi.status() != WL_CONNECTED)
	{
		delay(1000);
		Serial.println("Connecting to WiFi..");
	}

	// Print ESP32 Local IP Address
	Serial.println(WiFi.localIP());

	server.serveStatic("/index", SPIFFS, "index.html");
	server.serveStatic("/passcode", SPIFFS, "passcode.html");
	server.serveStatic("/mainMenu", SPIFFS, "mainMenu.html");
	server.serveStatic("/balance", SPIFFS, "balance.html");
	server.serveStatic("/withdraw", SPIFFS, "withdraw.html");
	server.serveStatic("/customWithdraw", SPIFFS, "customWithdraw.html");
	server.serveStatic("/receipt", SPIFFS, "receipt.html");

	// Route for root / web page
	server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
		request->send(SPIFFS, "/index.html", String(), false);
	});
	// Route to load style.css file
	server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request) {
		request->send(SPIFFS, "/style.css", "text/css");
	});
	server.on("/navigation", HTTP_GET, [](AsyncWebServerRequest *request) {
		request->send_P(200, "text/plain", getNavigation().c_str());
		page = 3;
	});
	server.on("/balanceNavigation", HTTP_GET, [](AsyncWebServerRequest *request) {
		request->send_P(200, "text/plain", getNavigation().c_str());
		page = 7;
	});
	// route to abort the transaction processes and return to index
	server.on("/abort", HTTP_GET, [](AsyncWebServerRequest *request) {
		request->send_P(200, "text/plain", getAbortCheck().c_str());
	});
	server.on("/rfidCheck", HTTP_GET, [](AsyncWebServerRequest *request) {
		request->send_P(200, "text/plain", getRfidCheck().c_str());		
		page = 1;
	});
	server.on("/loginCommand", HTTP_GET, [](AsyncWebServerRequest *request) {
		request->send_P(200, "text/plain", getLoginCommand().c_str());
		page = 2;
	});
	server.on("/passcodeLenght", HTTP_GET, [](AsyncWebServerRequest *request) {
		request->send_P(200, "text/plain", getPasscodeLenght().c_str());
	});
	server.on("/accountNumber", HTTP_GET, [](AsyncWebServerRequest *request) {
		request->send_P(200, "text/plain", getAccountNumber().c_str());
	});
	server.on("/getBalance", HTTP_GET, [](AsyncWebServerRequest *request) {
		request->send_P(200, "text/plain", getBalance().c_str());
	});
	server.on("/withdrawlNav", HTTP_GET, [](AsyncWebServerRequest *request) {
		request->send_P(200, "text/plain", getNavigation().c_str());
		page = 4;
	});
	server.on("/customAmount", HTTP_GET, [](AsyncWebServerRequest *request) {
		request->send_P(200, "text/plain", getCustomAmount().c_str());
		page = 5;
	});
	server.on("/customWithdrawlNav", HTTP_GET, [](AsyncWebServerRequest *request) {
		request->send_P(200, "text/plain", getNavigation().c_str());
	});
	server.on("/receiptNav", HTTP_GET, [](AsyncWebServerRequest *request) {
		request->send_P(200, "text/plain", getNavigation().c_str());
		page = 6;
	});


	server.on("/customWithdrawOptions", HTTP_GET, [](AsyncWebServerRequest *request) {
		request->send_P(200, "text/plain", getMenuTest().c_str());
	});

  /*
	server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest *request) {
		request->send_P(200, "text/plain", getHumidity().c_str());
	});

	server.on("/pressure", HTTP_GET, [](AsyncWebServerRequest *request) {
		request->send_P(200, "text/plain", getPressure().c_str());
	});
	*/
	// Start server
	
	server.begin();

	//initialize RFID reader
	SPI.begin(); // Init SPI bus
	mfrc522.PCD_Init(); // Init MFRC522
	mfrc522.PCD_DumpVersionToSerial(); // Show details of PCD - MFRC522 Card Reader details
}

#pragma region  // Functions for the diffrent pages

void rfidReader(){
	// Look for new cards
	
	// Select one of the cards
	if (!mfrc522.PICC_ReadCardSerial()){
		return;
	}

	// Vanaf hier tot regel 87 is voor het lezen van het rekeningNummer
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

void passcodeChecker(char customKey){	
	// check for input, check if passcode is 4 digits, check if 'A' has been pressed, check if password is correct, send lenght of passcode to passcodeLenght


	if (customKey == 'A' || customKey == 'B' || customKey == 'C' || customKey == 'D' || customKey == '*' || customKey == '#'){
		loginCommand = customKey;
		if (customKey == 'A' && passcode.length() == 4){
			//customKey == 'A' && dummyTempPasscode.length() == 4 && dummyTempPasscode == dummyPasscode
			switch (verifieer_pincode(passcode, rekeningNummer)){
				case 1:
					loginCommand = '1';
					break;
				case -1:
					loginCommand = '-1';
					break;
				case -2:
					loginCommand = '-2';
					break;
				case -3:
					loginCommand = '-3';
					break;
				case -4:
					loginCommand = '-4';
					break;
			}
		}
		else if (customKey == 'B'){
			passcode = "";
		}
	}
	else if (passcode.length() < 4){
		passcode += customKey;
		Serial.println(passcode);
	}
}

void navigationInput(char customKey){
	navigationKey = customKey;
	Serial.println("case3");
	Serial.print("navKey: ");
	Serial.println(navigationKey);
}

void dispense50(int value) {
  for (value; value != 0; value--) {
    Serial.println("50 word uitgeworpen");
    digitalWrite(motorForward50, HIGH);
    delay(1300);
    digitalWrite(motorForward50, LOW);
    digitalWrite(motorBackward50, HIGH);
    delay(800);
    digitalWrite(motorBackward50, LOW);
	uitStroom("50", "1", "ATM001", "ILoveMinderjarigen");
  }
}
void dispense20(int value) {
  for (value; value != 0; value--) {
    Serial.println("20 word uitgeworpen");
    digitalWrite(motorForward20, HIGH);
    delay(1500);
    digitalWrite(motorForward20, LOW);
    digitalWrite(motorBackward20, HIGH);
    delay(800);
    digitalWrite(motorBackward20, LOW);
	uitStroom("20", "1", "ATM001", "ILoveMinderjarigen");
  }
}
void dispense10(int value) {
  for (value; value != 0; value--) {
    Serial.println("10 word uitgeworpen");
    digitalWrite(motorForward10, HIGH);
    delay(1500);
    digitalWrite(motorForward10, LOW);
    digitalWrite(motorBackward10, HIGH);
    delay(800);
    digitalWrite(motorBackward10, LOW); 
	uitStroom("10", "1", "ATM001", "ILoveMinderjarigen");
  }
}
void dispense5(int value) {
  for (value; value != 0; value--) {
    Serial.println("5 word uitgeworpen");
    digitalWrite(motorForward5, HIGH);
    delay(1300);
    digitalWrite(motorForward5, LOW);
    digitalWrite(motorBackward5, HIGH);
    delay(600);
    digitalWrite(motorBackward5, LOW);
	uitStroom("5", "1", "ATM001", "ILoveMinderjarigen");
  }
}

void dispenseMoney(){
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

	// int increament = 2;

	// for(int i = 1; i < 9; i += increament){
	// 	Serial.print("Note array: ");
	// 	Serial.print(noteArray[i]);
	// 	if(noteArray[i] == 0){
	// 		break;
	// 	}else{
	// 		String noteAmount = String(noteArray[i]);
	// 		String noteWorth = String(noteArray[i+1]);
	// 		uitStroom(noteWorth, noteAmount, "ATM001", "ILoveMinderjarigen");
	// 	}
	// }

	abortCheck = true;
}

void billSelection(){
	//Serial.println(atoi(((String)billCombinations[comb].charAt(1)).c_str()));
	int noteArrayConstructorStep = 0;
	String billConstructor = "";
	
	//Serial.println(atoi(((String)billCombinations[billCombinationSelection].charAt(1)).c_str()));

	for(int i = 0; i < 6; i++){
		if(noteArrayConstructorStep == 0 || noteArrayConstructorStep == 3){
			int combNumber = atoi(((String)billCombinations[billCombinationSelection].charAt(i)).c_str());
			switch (i){
				case 0:
					noteArray[1] = combNumber;
					break;
				case 3:
					noteArray[3] = combNumber; 
					break;
			}
			noteArrayConstructorStep++;
		}else if(noteArrayConstructorStep == 1 || noteArrayConstructorStep == 4){
			billConstructor += billCombinations[billCombinationSelection].charAt(i);
			noteArrayConstructorStep++;
		}else{
			billConstructor += billCombinations[billCombinationSelection].charAt(i);
			switch (i){
			case 2:
				noteArray[2] = atoi(((String)billConstructor).c_str());
				billConstructor = "";
				break;
			case 5:
				noteArray[4] = atoi(((String)billConstructor).c_str());
				billConstructor = "";
				break;
			}
			noteArrayConstructorStep++;
		}
	}
	for(int i = 5; i < 9; i++){
		noteArray[i] = 0;
	}
	for(int i = 0; i < 9; i++){
		Serial.println(noteArray[i]);
	}
}

void billsInMachine(){
	String bills[4] = {"5", "10", "20", "50"};
	for(int i = 0; i < 4; i++){
		billsAvailable[i] = aantalBriefjes(bills[i], "ATM001", "ILoveMinderjarigen");
	}
}



void customAmountBillConstructor(int customAmountInt, String customAmountStr, int customAmountLenght){
	Serial.println("customAmountBillConstructor");
	int customAmountMaster = customAmountInt;	// a copy of the amount of money chosen
	int customAmount = customAmountMaster;
	int locationArray01 = 1;
	int locationArray02 = 1;
	int locationArray03 = 1;
	for(int i = 0; i < 9; i ++){
		customNoteArray01[i] = 0;
	}
	for(int i = 0; i < 9; i ++){
		customNoteArray02[i] = 0;
	}
	for(int i = 0; i < 9; i ++){
		customNoteArray03[i] = 0;
	}
	customNoteArray01[0] = customAmount;
	customNoteArray02[0] = customAmount;
	customNoteArray03[0] = customAmount;
	int divideCalculation;	// int to store calculations in.
	boolean endsOn5 = false;	// boolean is true when custom amount ends on 5
	int billOptions[4] = {50, 20, 10, 5};	// the diffrent bills which can be selected
	Serial.print("temp algo 2: ");
	for(int i = 0; i < 9; i++){
		Serial.print(customNoteArray02[i]);
		Serial.print(" ");
	}
	Serial.println(" ");
	if(customAmountStr.charAt(customAmountLenght) == '5'){		// checks if the custom amount ends on a 5, so yes, removes the 5 and adds it to the array
		customNoteArray01[locationArray01] = 1;
		customNoteArray01[locationArray01 + 1] = 5;
		customNoteArray02[locationArray02] = 1;
		customNoteArray02[locationArray02 + 1] = 5;
		customNoteArray03[locationArray03] = 1;
		customNoteArray03[locationArray03 + 1] = 5;
		customAmountMaster -= 5;
		locationArray01 += 2;
		locationArray02 += 2;
		locationArray03 += 2;
		endsOn5 = true;
	}

	if(customAmountMaster != 0){	
		#pragma region	//algorithem 1
		// constructs the first array with bills, least amount of bills.
		customAmount = customAmountMaster;
		for(int i = 0; i < 3; i++){		
			divideCalculation = customAmount / billOptions[i];
			if(divideCalculation >= 1){
				Serial.println(billOptions[i]+" divide");
				customNoteArray01[locationArray01] = divideCalculation;
				customNoteArray01[locationArray01 + 1] = billOptions[i];
				customAmount -= billOptions[i] * divideCalculation;
				locationArray01 += 2;
			}
		}
		#pragma endregion 
		#pragma region	//algorithem 2
		// constructs the second array with bills. works in 2 steps:
		// step 1: divides the amount, and gives the first half in as little as possible notes
		// step 2: gives the second half completely in the notes that are next in line to the biggest note from step 1, if we get stuck -> onto the next note.
		Serial.println("algo02");
		customAmount = customAmountMaster;
		int half1 = customAmount / 2;
		int half2 = half1;
		Serial.print("half1: ");
		Serial.println(half1);
		if(half1 % 10 == 5){
			if(endsOn5 == true){
				Serial.println("Ends on 5");
				customNoteArray02[locationArray02 - 2] = 3;
			}else{
				customNoteArray02[locationArray02] = 2;
				customNoteArray02[locationArray02 + 1] = 5;
				locationArray02 += 2;
			}
			half1 -= 5;
			half2 -= 5;
			Serial.print("half1: ");
			Serial.println(half1);
		}
		if(half1 != 0){
			// step 1:
			boolean firstRun = false;
			int biggestBillUsed;
			Serial.print("step1: ");
			for(int i = 0; i < 3 != 0; i++){
				divideCalculation = half1 / billOptions[i];
				if(divideCalculation >= 1){	
					Serial.print(billOptions[i]);
					Serial.println(" divider");
					if(firstRun == false){
						biggestBillUsed = i;
						firstRun = true;
					}
					customNoteArray02[locationArray02] = divideCalculation;
					customNoteArray02[locationArray02 + 1] = billOptions[i];
					half1 -= billOptions[i] * divideCalculation;
					locationArray02 += 2;
				}
			}

			// step 2:
			Serial.println("step2: ");
			for(int i = 1; i < 3; i++){
				boolean inArray = false;
				Serial.print("for run: ");
				Serial.println(i);
				divideCalculation = half2 / billOptions[biggestBillUsed + i];
				if(divideCalculation >= 1){
					Serial.print(billOptions[i]);
					Serial.println(" divider");
					//check if bill is in array. yes -> add to that location, no -> make new entry
					for(int u = 1; u < 9; u++){
						if(u % 2 == 0 && customNoteArray02[u] == billOptions[biggestBillUsed + i]){
							int tempBillAmount = customNoteArray02[u - 1];
							tempBillAmount += divideCalculation;
							customNoteArray02[u - 1] = tempBillAmount;
							half2 -= billOptions[biggestBillUsed + i] * divideCalculation;
							inArray = true;
						}
					}
					if(inArray == false){
						Serial.print("divide calc: ");
						Serial.println(divideCalculation);
						customNoteArray02[locationArray02] = divideCalculation;
						customNoteArray02[locationArray02 + 1] = billOptions[biggestBillUsed + i];
						half2 -= billOptions[biggestBillUsed + i] * divideCalculation;
						locationArray02 += 2;
					}
				}
			}
		}
		#pragma endregion

		#pragma region	//algorithem 3
		// constructs the fourth array with bills, all 10's.
		customAmount = customAmountMaster;
		while(customAmount != 0){
			for(int i = 2; i > -1; i--){
				divideCalculation = customAmount / billOptions[i];
				if(divideCalculation >= 1){
					if(divideCalculation < 11){
						customNoteArray03[locationArray03] = divideCalculation;
						customNoteArray03[locationArray03 + 1] = billOptions[i];
						locationArray03 += 2;
						customAmount -= divideCalculation * billOptions[i];
					}
				}
			}
		}
		#pragma endregion


		// customNoteArray04[locationArray04] = divideCalculation;
		// customNoteArray04[locationArray04 + 1] = billOptions[2];
		// locationArray04 += 2;
	}
	for(int i = locationArray01; i < 9; i ++){
		customNoteArray01[i] = 0;
	}
	for(int i = locationArray02; i < 9; i ++){
		customNoteArray02[i] = 0;
	}
	for(int i = locationArray03; i < 9; i ++){
		customNoteArray03[i] = 0;
	}
	Serial.print("algo 1: ");
	for(int i = 0; i < 9; i++){
		Serial.print(customNoteArray01[i]);
		Serial.print(" ");
	}
	Serial.println(" ");
	Serial.print("algo 2: ");
	for(int i = 0; i < 9; i++){
		Serial.print(customNoteArray02[i]);
		Serial.print(" ");
	}
	Serial.println(" ");
	Serial.print("algo 3: ");
	for(int i = 0; i < 9; i++){
		Serial.print(customNoteArray03[i]);
		Serial.print(" ");
	}
	Serial.println(" ");
/*

*/

}

void billArrayStringConstructor(int billArray[9], int arrayNumber){
	//&euro;10
	
	//String tempArrayString;
	int increament = 2;
	if(billArray[0] == 0){
		billCombinationString[arrayNumber] = "";
	}else{
		for(int u = 1; u < 9; u += increament){
			if(billArray[u] == 0){
				break;
			}
			if(billCombinationString[arrayNumber] == ""){
				billCombinationString[arrayNumber] += leftTabs[arrayNumber];
			}
			String billAmount = String(billArray[u]);
			String value = String(billArray[u+1]);
			billCombinationString[arrayNumber] += billAmount+" x "+currency+value+" ";
		}	
	}

}

void billArrayChecker(int billArray[9], int arrayNumber){	// function to check if the bills selected are available.

	int increament = 2;
	Serial.println(billArray[0]);
	for(int i = 1; i < 9; i += increament){
		//String value = String(billArray[i+1]);
		if(billArray[i] == 0){
			break;
		}else if(billsAvailable[billsAvailableIndex[billArray[i+1]]] < billArray[i]){
			Serial.print("Er zijn te weinig briefjes van: ");
			Serial.println(billArray[i+1]);
			billArray[0] = 0;
			break;
		}
	}
	Serial.println(billArray[0]);
	billArrayStringConstructor(billArray, arrayNumber);
	return;
}

void withdrawlMenu(char customKey){
	//Serial.println("case 4");
	
	Serial.print("customKey: ");
	Serial.println(customKey);
	Serial.print("Withdraw step: ");
	Serial.println(withdrawStep);
	if(customKey == '1' || customKey == '4' || customKey == '7' || customKey == '*'){
		if(withdrawStep == 0 || withdrawStep == 1){
			navigationKey = customKey;
			switch (customKey){
				case '1':
					if(withdrawStep == 0){
						billsInMachine();
						customAmountBillConstructor(10, "10", 1);
					}else{
						Serial.println("array: 1");
						for(int i = 0; i < 9; i++){
							noteArray[i] = customNoteArray01[i];
							Serial.print(noteArray[i]);
						}
						Serial.println("");
						customAmount = billCombinationString[0].substring(2, billCombinationString[0].length());
					}
					break;
				case '4':
					if(withdrawStep == 0){
						billsInMachine();
						customAmountBillConstructor(30, "30", 1);
					}else{		
						Serial.println("array: 2");
						for(int i = 0; i < 9; i++){
							noteArray[i] = customNoteArray02[i];
							Serial.print(noteArray[i]);
						}
						Serial.println("");
						customAmount = billCombinationString[2].substring(2, billCombinationString[2].length());
					}
					break;
				case '7':
					if(withdrawStep == 0){
						billsInMachine();
						customAmountBillConstructor(70, "70", 1);
					}else{						
						Serial.println("array: 3");
						for(int i = 0; i < 9; i++){
							noteArray[i] = customNoteArray03[i];
							Serial.print(noteArray[i]);
						}
						Serial.println("");
						customAmount = billCombinationString[3].substring(2, billCombinationString[3].length());
					}
					break;
				case '*':
					if(withdrawStep == 0){
						withdrawStep = 0;
						CustomWithdrawOption = "c0000";	//tells gui to switch to custom withdraw menu
						return;
					}else{
						billCombinationSelection += "*";
					}
					break;
			}
			if(withdrawStep == 0){
				billArrayChecker(customNoteArray01, 0);
				billArrayChecker(customNoteArray02, 1);
				billArrayChecker(customNoteArray03, 2);
				withdrawStep = 1;
			}else{
				if(customKey == '1' || customKey == '4' || customKey == '7'){
					for(int i = 0; i < 3; i++){
						billCombinationString[i] = leftTabs[i];
					}
					withdrawStep = 2;
				}
			}

		}
	}else if(customKey == 'A' || customKey == 'B' || customKey == 'C' || customKey == 'D'){
		navigationKey = customKey;
		if(customKey == 'A' && withdrawStep == 2){
			Serial.println("option set to d");
			CustomWithdrawOption = "d0000";	// tells the gui to switch to the receipt menu
		}
		if(customKey == 'B' || customKey == 'C'){
			CustomWithdrawOption = "q0000";// tells gui to reset
			billCombinationSelection = "";
			withdrawStep = 0;
			for(int i = 0; i < 3; i++){
				billCombinationString[i] = leftTabs[i];
			}
		}
	}
}

void customAmountMenu(char customKey){

	Serial.print("customKey: ");
	Serial.println(customKey);
	Serial.print("Withdraw step: ");
	Serial.println(withdrawStep);
	if(customKey == 'A' || customKey == 'B' || customKey == 'C' || customKey == 'D'){
		navigationKey = customKey;
		int customAmountLenght = customAmount.length()-1;
		if(customKey == 'A'){
			Serial.println("key: A");
			Serial.println(customAmount.charAt(customAmountLenght));
			if(withdrawStep == 2){
				Serial.println("option set to d");
				CustomWithdrawOption = "d";	// tells the gui to switch to the receipt menu
			}
			if(wrongInput == true){
				Serial.println("wrong input yeye");
				customAmount = "";
				wrongInput = false;
				return;
			}
			if(customAmount.charAt(customAmountLenght) == '5' || customAmount.charAt(customAmountLenght) == '0'){
				if(customAmount.charAt(customAmountLenght) == '5' && aantalBriefjes("5", "ATM001", "ILoveMinderjarigen") == 0){
					customAmount = "Helaas zijn de 5$ biljetten momenteel op, probeer later weer.";
					wrongInput = true;
				}else if(withdrawStep == 0){
					int customAmountInt = atoi(((String)customAmount).c_str());
					Serial.print("customAmountInt: ");
					Serial.println(customAmountInt);
					Serial.print("customAmountLenght: ");
					Serial.println(customAmountLenght);
					billsInMachine();
					customAmountBillConstructor(customAmountInt, customAmount, customAmountLenght);
					billArrayChecker(customNoteArray01, 0);
					billArrayChecker(customNoteArray02, 1);
					billArrayChecker(customNoteArray03, 2);
					withdrawStep = 1;
				}

			}else if(customAmount != ""){
				customAmount = "Zorg dat het bedrag eindigt op een 5 of 0.";
				wrongInput = true;
			}
		}
		switch (customKey)
		{
			case 'B':
				customAmount = "";
				wrongInput = false;
				withdrawStep = 0;
				for(int i = 0; i < 3; i++){
					billCombinationString[i] = "";
				}
				break;
			case 'C':
				if(withdrawStep == 0){
					customAmount = customAmount.substring(0, customAmount.length()-1);	//removes last number of the amount that has been input
				}else{
					customAmount = "";
					withdrawStep = 0;
					for(int i = 0; i < 3; i++){
						billCombinationString[i] = leftTabs[i];
					}
					CustomWithdrawOption = "q";	// resets the display message to the original
				}
				break;
		}
	}else if(customKey != '*' && customKey != '#' && wrongInput != true){
		if(withdrawStep == 1){
			switch (customKey){
				case '1':
					Serial.println("array: 1");
					for(int i = 0; i < 9; i++){
						noteArray[i] = customNoteArray01[i];
						Serial.print(noteArray[i]);
					}
					Serial.println("");
					customAmount = billCombinationString[0].substring(2, billCombinationString[0].length());

					break;
				case '4':
					Serial.println("array: 2");
					for(int i = 0; i < 9; i++){
						noteArray[i] = customNoteArray02[i];
						Serial.print(noteArray[i]);
					}
					Serial.println("");
					customAmount = billCombinationString[2].substring(2, billCombinationString[2].length());
					break;
				case '7':
					Serial.println("array: 3");
					for(int i = 0; i < 9; i++){
						noteArray[i] = customNoteArray03[i];
						Serial.print(noteArray[i]);
					}
					Serial.println("");
					customAmount = billCombinationString[3].substring(2, billCombinationString[3].length());
					break;
				default:
					break;
			}
			if(customKey == '1' || customKey == '4' || customKey == '7'){
				for(int i = 0; i < 3; i++){
					billCombinationString[i] = leftTabs[i];
				}
				withdrawStep = 2;
			}
		}else{
			customAmount += customKey;
			Serial.println(customAmount);
		}
	}
}



void receiptPrinter(){
	Serial.println("Now printing");
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

	String passNumber = "************";
	for(int i = 12; i < 16; i ++){
		passNumber += rekeningNummer.charAt(i);
	}
	Serial.println(passNumber);
    printer.println(passNumber);
    printer.println();
  
   //Het opgenomen bedrag
    printer.setSize('M');
    printer.print("Opgenomen bedrag: €100 ");
    printer.println(noteArray[0]);
    printer.println();
    
    //Het bedankt bericht
    printer.justify('C');
    printer.setSize('M');
    printer.println(F("Tot ziens"));
    printer.println(F("bij BOEJIEBANK!\n"));
    printer.println();
    printer.println();
	Serial.println("Done printing");

}

void receiptMenu(char customKey){
	switch (customKey){
		case 'A':
			/* receipt constructor, 
			printer, 
			dispense money */
			receiptPrinter();
			geldOpnemen();
			dispenseMoney();
			break;
		case 'B':
			//dispense money
			geldOpnemen();
			dispenseMoney();
			break;
	}
}

void quickWithdraw(){
	billsInMachine();
	customAmountBillConstructor(70, "70", 1);
	billArrayChecker(customNoteArray01, 0);
	billArrayChecker(customNoteArray02, 1);
	billArrayChecker(customNoteArray03, 2);
	if(customNoteArray01[0] != 0){
		for(int i = 0; i < 9; i++){
			noteArray[i] = customNoteArray01[i];
			Serial.print(noteArray[i]);
		}
	}else if(customNoteArray02[0] != 0){
		for(int i = 0; i < 9; i++){
			noteArray[i] = customNoteArray02[i];
			Serial.print(noteArray[i]);
		}
	}else if(customNoteArray03[0] != 0){
		for(int i = 0; i < 9; i++){
			noteArray[i] = customNoteArray03[i];
			Serial.print(noteArray[i]);
		}
	}else{
		// print error message that the machine is out of bills
		return;
	}

	geldOpnemen();
	dispenseMoney();
}

#pragma endregion

void timerControl(){	// function to abort if the user has been inactive for too long
	
	Serial.print("time: ");
	Serial.println(millis()-sessionTime);
	switch(page){
		case 2:
			if(millis()-sessionTime > pincodeTimeOut){
				abortCheck = true;
				timerRunning = false;
				page = 1;
			}
			break;
		default:
			if(millis()-sessionTime > generalTimeOut){
				abortCheck = true;
				timerRunning = false;
				page = 1;
			}
	}
	
}

void loop(){	//void main

	if(page == 1){
		if (mfrc522.PICC_IsNewCardPresent()){
		rfidReader();
		rfidCheck = true;
		sessionTime = millis();
		Serial.println("--------------------------------------------------------------loc1");
		delay(200);
		}
	}else{
		//timerControl();	//function to start timer && check the spend time
		char customKey = customKeypad.getKey();
  		if (customKey){
			sessionTime = millis();	//resets the inactivity timer
			switch (page){
				case 2:	//--------------------Take the keypad input for the passcode
					passcodeChecker(customKey);
					break;
				case 3:	//---------------- take keypad input for navigation
					navigationInput(customKey);
					if(customKey == '7'){
						quickWithdraw();
					}
					break;
				case 4://-----------------withdraw menu
					withdrawlMenu(customKey);
					vTaskDelay(10);
					break;
				case 5:
					customAmountMenu(customKey);
					break;
				case 6://-------------receipt menu
					receiptMenu(customKey);
					break;
				case 7:
					navigationInput(customKey);
					break;
			}
			if(customKey == 'D'){
				abortCheck = true;
			}
  		}
	}
	yield();
}