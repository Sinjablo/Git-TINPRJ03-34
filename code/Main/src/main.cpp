#include <arduino.h>
#include <stdio.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>
#include <Wire.h>
#include <iostream>
#include <string>

const int irTest = D1;

// Replace with your network credentials
//const char *ssid = "ASUS1424";
//const char *password = "MaJaNe14245.";

//const char *ssid = "Tesla IoT";
//const char *password = "fsL6HgjN";

const char *ssid = "LaptopieVanSander";
const char *password = "KrijgsheerSander";

//const char *ssid = "lenovolaptop";
//const char *password = "jarno123";

using namespace std;  

String x;
// Set LED GPIO
const int ledPin = 2;
// Stores LED state
String ledState;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

//Variables
// variables to return to the GUI website
bool abortCheck = false;
bool rfidCheck = false;
bool passcodeCheck = false;
String lastName = "Vuijk";
bool withdrawCheck = false;
bool balanceCheck = false;


//--------------------------------------String return functions for the webserver to switch pages

String getAbortCheck(){
	bool tempAbortCheck = abortCheck;
	abortCheck = false;
	return String(tempAbortCheck);
}
String getIrTest(){
	int x = digitalRead(irTest);
	return String(x);
}
String getRfidCheck(){
	bool tempAbortCheck = abortCheck;
	abortCheck = false;
	return String(tempAbortCheck);
	
}
String getPasscodeCheck(){
	bool tempPasscodeCheck = passcodeCheck;
	passcodeCheck = false;
	return String(tempPasscodeCheck);
}
String getLastName(){
	return lastName;
}
String getWithdrawCheck(){
	bool tempWithdrawCheck = withdrawCheck;
	withdrawCheck = false;
	return String(tempWithdrawCheck);
}
String getBalanceCheck(){
	bool tempBalanceCheck = balanceCheck;
	balanceCheck = false;
	return String(tempBalanceCheck);
}

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
}

void setup(){
	// Serial port for debugging purposes
	Serial.begin(115200);
	pinMode(ledPin, OUTPUT);
	pinMode(irTest, OUTPUT);

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
	// Route for root / web page
	server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
		request->send(SPIFFS, "/index.html", String(), false, processor);
	});

	// Route to load style.css file
	server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request) {
		request->send(SPIFFS, "/style.css", "text/css");
	});

	server.on("/abort", HTTP_GET, [](AsyncWebServerRequest *request) {
		request->send_P(200, "text/plain", getAbortCheck().c_str());
	});
	server.on("/rfidCheck", HTTP_GET, [](AsyncWebServerRequest *request) {
		request->send_P(200, "text/plain", getIrTest().c_str());		
	});
	server.on("/passcodeCheck", HTTP_GET, [](AsyncWebServerRequest *request) {
		request->send_P(200, "text/plain", getIrTest().c_str());
	});
	server.on("/lastName", HTTP_GET, [](AsyncWebServerRequest *request) {
		request->send_P(200, "text/plain", getLastName().c_str());
	});
	server.on("/witdrawCheck", HTTP_GET, [](AsyncWebServerRequest *request) {
		request->send_P(200, "text/plain", getBalanceCheck().c_str());
	});
	server.on("/balanceCheck", HTTP_GET, [](AsyncWebServerRequest *request) {
		request->send_P(200, "text/plain", getWithdrawCheck().c_str());
	});
	/*
	server.on("/passcodeLenght", HTTP_GET, [](AsyncWebServerRequest *request) {
		request->send_P(200, "text/plain", getPasscodeLenght().c_str());
	});

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

	if(digitalRead(D1) == HIGH){
		abortCheck = true;
	}
	
}