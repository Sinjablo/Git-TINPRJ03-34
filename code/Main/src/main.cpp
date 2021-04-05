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


// Replace with your network credentials

const char *ssid = "ASUS1424";
const char *password = "MaJaNe14245.";

//const char *ssid = "Tesla IoT";
//const char *password = "fsL6HgjN";

//const char *ssid = "LaptopieVanSander";
//const char *password = "KrijgsheerSander";

//const char *ssid = "lenovolaptop";
//const char *password = "jarno123";

//--------- Setup keypad----
const byte ROWS = 4; 
const byte COLS = 4; 

char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {23, 22, 3, 21}; 
byte colPins[COLS] = {19, 18, 16, 17};
Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 
//----------End keypad setUp

int page;

using namespace std;  

//dummy variables:
String dummyPasscode = "7777";
String dummyTempPasscode;
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
	//int tempNavigationKey = 0;

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
		page = 2;
	});
	// route to abort the transaction processes and return to index
	server.on("/abort", HTTP_GET, [](AsyncWebServerRequest *request) {
		request->send_P(200, "text/plain", getAbortCheck().c_str());
	});
	server.on("/rfidCheck", HTTP_GET, [](AsyncWebServerRequest *request) {
		request->send_P(200, "text/plain", getRfidCheck().c_str());		
	});
	server.on("/loginCommand", HTTP_GET, [](AsyncWebServerRequest *request) {
		request->send_P(200, "text/plain", getLoginCommand().c_str());
		page = 1;
	});
	server.on("/passcodeLenght", HTTP_GET, [](AsyncWebServerRequest *request) {
		request->send_P(200, "text/plain", getPasscodeLenght().c_str());
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
}

void loop(){
	char customKey = customKeypad.getKey();
  	if (customKey){
		
		switch (page)
		{
		case 1:
		  	// check for input, check if passcode is 4 digits, check if 'A' has been pressed, check if password is correct, send lenght of passcode to passcodeLenght
			if(customKey == 'A' || customKey == 'B' || customKey == 'C' || customKey == 'D' || customKey == '*' || customKey == '#'){
				loginCommand = customKey;
				if(customKey == 'A' && dummyTempPasscode.length() == 4 && dummyTempPasscode == dummyPasscode){
					loginCommand = '1';
					dummyTempPasscode = "";
				}else if(customKey == 'B'){
					dummyTempPasscode = "";
				}
			}else if(dummyTempPasscode.length() < 4){
				dummyTempPasscode += customKey;
				Serial.println(dummyTempPasscode);
			}
		  	break;
		case 2:
			  navigationKey = customKey;
		  	break;
		}
		if(customKey == 'D'){
			abortCheck = true;
		}
  	}

	delay(1);
}