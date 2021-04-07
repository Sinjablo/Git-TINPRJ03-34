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

//const char *ssid = "Tesla IoT";
//const char *password = "fsL6HgjN";

//const char *ssid = "LaptopieVanSander";
//const char *password = "KrijgsheerSander";

//const char *ssid = "lenovolaptop";
//const char *password = "jarno123";

//const char *ssid = "VielvoyeResidence24GHz";
//const char *password = "Oli/5iN-dR=88#VRGHZ#24";

//Access point credentials

const char* host = "http://192.168.178.73"; //IPv4 adress hosting laptop/server
String get_host = "http://192.168.178.73"; //same as above

String userPasscode = "7777";
String key = "de3w2jbn7eif1nw9e";

//--------- Setup keypad----
const byte ROWS = 4; 
const byte COLS = 4; 

char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
//byte rowPins[ROWS] = {23, 22, 3, 21}; 
//byte colPins[COLS] = {19, 18, 16, 17};
byte rowPins[ROWS] = {22, 3, 5, 17}; 
byte colPins[COLS] = {16, 4, 15, 2};
Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 
//----------End keypad setUp
//----------Setup RFID reader-------
const int RST_PIN = 13; // Reset pin
const int SS_PIN = 21; // Slave select pin
 
MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance
//-------------end rfid setup

int page;

using namespace std;  

//dummy variables:
String dummyPasscode = "7777";
String dummyTempPasscode;
String dummyRekeningnummer = "NL21HAHA010032003";
String x;
char g;
const int tempBtn = 4;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

//Variables
// variables to return to the GUI website
char navigationKey;
bool abortCheck = false;
bool rfidCheck = false;
char loginCommand = '0';
String lastName = "Vuijk";
String passcodeLenght;

//--------------------------------------String return functions for the webserver to switch pages

String getAbortCheck(){
	bool tempAbortCheck = abortCheck;
	abortCheck = false;
	if(tempAbortCheck == true){
		dummyTempPasscode = "";
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
	bool tempAbortCheck = abortCheck;
	abortCheck = false;
	return String(tempAbortCheck);
	
}
String getLoginCommand(){
	char tempPasscodeCheck = loginCommand;
	loginCommand = '0';
	return String(tempPasscodeCheck);
}

String getPasscodeLenght(){
	return String(dummyTempPasscode.length());
}

String getAccountNumber(){
	String x = "NL04BOEB34254g54";
	return String(x);
}
String getBalance(){
	int x = 6547;
	return String(x);
}
/*
// ONLY FOR START-UP I THINK
String processor(const String &var){
	Serial.println(var);
	if (var == "STATE")
	{
		if (digitalRead(ledPin))
		{
			ledState = "ON";
		}
		else
		{
			ledState = "OFF";
		}
		Serial.print(ledState);
		return ledState;
	}

}*/

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

void setup(){
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

void loop(){
	char customKey = customKeypad.getKey();
  	if (customKey){
		
		switch (page){
			case 1:	//------------------Check for a RFID card & read it out
				// Look for new cards
				if (!mfrc522.PICC_IsNewCardPresent()){
					return;
				}
				// Select one of the cards
				if (!mfrc522.PICC_ReadCardSerial()){
					return;
				}
				// Dump debug info about the card; PICC_HaltA() is automatically called
				byte readCard[7]; // This is our byte array to store UID mind that there are 4 and 7 bytes long UID
				Serial.println("Scanned PICC's UID:");
				for (int i = 0; i < mfrc522.uid.size; i++){
					readCard[i] = mfrc522.uid.uidByte[i];
					Serial.print(readCard[i], HEX);
				}
				Serial.println("");
				mfrc522.PICC_HaltA();
				mfrc522.PCD_StopCrypto1();

			case 2:	//--------------------Take the keypad input for the passcode
				// check for input, check if passcode is 4 digits, check if 'A' has been pressed, check if password is correct, send lenght of passcode to passcodeLenght
				if (customKey == 'A' || customKey == 'B' || customKey == 'C' || customKey == 'D' || customKey == '*' || customKey == '#'){
					loginCommand = customKey;
					if (customKey == 'A' && dummyTempPasscode.length() == 4 && verifieer_pincode(dummyTempPasscode, dummyRekeningnummer) == true){
						//customKey == 'A' && dummyTempPasscode.length() == 4 && dummyTempPasscode == dummyPasscode
						loginCommand = '1';
						dummyTempPasscode = "";
					}
					else if (customKey == 'B'){
						dummyTempPasscode = "";
					}
				}
				else if (dummyTempPasscode.length() < 4){
					dummyTempPasscode += customKey;
					Serial.println(dummyTempPasscode);
				}
				break;
			case 3:	//---------------- take keypad input for navigation
				navigationKey = customKey;
				break;
		}
		if(customKey == 'D'){
			abortCheck = true;
		}
  	}
	
	// Look for new cards
	if (!mfrc522.PICC_IsNewCardPresent()){
		return;
	}

	// Select one of the cards
	if (!mfrc522.PICC_ReadCardSerial()){
		return;
	}

	// Dump debug info about the card; PICC_HaltA() is automatically called
	//mfrc522.PICC_DumpDetailsToSerial(&(mfrc522.uid));
	byte readCard[7]; // This is our byte array to store UID mind that there are 4 and 7 bytes long UID
	//String UIDString = "";
	char UIDChar[7];
	Serial.println("Scanned PICC's UID:");
	// for (int i = 0; i < mfrc522.uid.size; i++){
	// 	readCard[i] = mfrc522.uid.uidByte[i];
	// 	Serial.print(readCard[i], HEX);
	// }
	for (int i = 0; i < mfrc522.uid.size; i++){
		//readCard[i] = mfrc522.uid.uidByte[i];
		//Serial.print(readCard[i], HEX);
		UIDChar[i] += mfrc522.uid.uidByte[i];
	}
	Serial.println(UIDChar);
	Serial.println("");
	mfrc522.PICC_HaltA();
	mfrc522.PCD_StopCrypto1();

	delay(1);
}