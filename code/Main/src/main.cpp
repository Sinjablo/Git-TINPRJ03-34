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


// Replace with your network credentials

const char *ssid = "ASUS1424";
const char *password = "MaJaNe14245.";

// const char *ssid = "Tesla IoT";
// const char *password = "fsL6HgjN";

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
byte colPins[COLS] = {16, 4, 15, 2};
byte rowPins[ROWS] = {22, 3, 5, 17}; 
Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 
//----------End keypad setUp
//----------Setup RFID reader-------
const int RST_PIN = 13; // Reset pin
const int SS_PIN = 21; // Slave select pin
 
MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance
//-------------end rfid setup


using namespace std;  

//dummy variables:
String dummyPasscode = "7777";
String dummyTempPasscode;
String dummyRekeningnummer;
String xa = "";
String x;
char g;
const int tempBtn = 4;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);


//Variables
int page = 1;
bool withdrawSelection = false;
String rekeningNummer;
String uid;
String passcode;
String tempPasscode;
int endSession;
long sessionTime = 0;
String balance;
bool timerRunning = false;

// variables to return to the GUI website
char navigationKey;
bool abortCheck = false;
bool rfidCheck = false;
char loginCommand = '0';
String lastName = "Vuijk";
String passcodeLenght;

bool verifieer_pincode(String passcode, String accountNumber){
    
    //WiFiClient client = server.available();
		
    HTTPClient http;
    String url = get_host+"/verificatie.php?"+"sltl="+key+"&mgrkn="+accountNumber+"&mgpc="+passcode;
    Serial.println(url);
    
    http.begin(url);
    
    //GET method
    int httpCode = http.GET();
    String payload = http.getString();
    Serial.println(payload);
    http.end(); 
	if(payload == "1"){
		return true;
	}else{
		return false;
	}
  
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
#pragma endregion

void setup(){	// void setup
	// Serial port for debugging purposes
	Serial.begin(115200);
	pinMode(tempBtn, OUTPUT);

	

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
		// if(page == 1){
		// 	endSession = 0;
		// }
		// endSession += 1;
		// if(endSession == 18){
		// 	endSession = 0;
		// 	abortCheck = true;
		// }
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
		//Serial.println("withdrawamount");
		page = 4;
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
		if (customKey == 'A' && passcode.length() == 4 && verifieer_pincode(passcode, rekeningNummer) == true){
			//customKey == 'A' && dummyTempPasscode.length() == 4 && dummyTempPasscode == dummyPasscode
			loginCommand = '1';
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
	if(navigationKey == '4'){
		page = 4;
	}
}

void withdrawlMenu(char customKey){
	Serial.println("case 4");
	if(withdrawSelection == false){
		navigationKey = customKey;
		if(customKey == '1' || customKey == '4' || customKey == '7' || customKey == '*'){
			withdrawSelection = true;
			Serial.println("1,2,4*");
		}
	}else if(customKey == 'A' || customKey == 'B' || customKey == 'C' || customKey == 'D'){
		navigationKey = customKey;
		if(customKey == 'C'){
		withdrawSelection = false;
		}
	}
}
#pragma endregion

void timerControl(){	// function to abort if the user has been inactive for too long
	
	Serial.print("time: ");
	Serial.println(millis()-sessionTime);
	switch(page){
		case 2:
			if(millis()-sessionTime > 10000){
				abortCheck = true;
				timerRunning = false;
				page = 1;
			}
			break;
		default:
			if(millis()-sessionTime > 45000){
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
		timerControl();	//function to start timer && check the spend time
		char customKey = customKeypad.getKey();
  		if (customKey){
			sessionTime = millis();	//resets the inactivity timer
			switch (page){
				case 2:	//--------------------Take the keypad input for the passcode
					passcodeChecker(customKey);
					break;
				case 3:	//---------------- take keypad input for navigation
					navigationInput(customKey);
					break;
				case 4://-----------------withdraw menu
					withdrawlMenu(customKey);
					break;
			}
			if(customKey == 'D'){
				abortCheck = true;
			}
  		}
	}
	yield();
}
