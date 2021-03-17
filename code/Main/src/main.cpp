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
const char *ssid = "ASUS1424";
const char *password = "MaJaNe14245.";

using namespace std;  

String y;
// Set LED GPIO
const int ledPin = 2;
// Stores LED state
String ledState;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

String getIrTest()
{
	int x = digitalRead(irTest);
	Serial.println(x);
	return String(x);
}
String getPasscode(){
	int passcodeLenght;
    //passcodeLenght = tostring(y);
	
    passcodeLenght = y.lenght();

	switch (passcodeLenght)
	{
	case 1:
		document.getElementById("humidity").innerHTML = "*";
		break;
	case 2:
		document.getElementById("humidity").innerHTML = "**";
		break;
	case 3:
		document.getElementById("humidity").innerHTML = "***";
		break;
	case 4:
		document.getElementById("humidity").innerHTML = "****";
		break;
	}
}
/*
String getHumidity()
{
	float humidity = bme.readHumidity();
	Serial.println(humidity);
	return String(humidity);
}

String getPressure()
{
	float pressure = bme.readPressure() / 100.0F;
	Serial.println(pressure);
	return String(pressure);
}
*/

// Replaces placeholder with LED state value
// ONLY FOR START-UP I THINK
String processor(const String &var)
{
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


void setup()
{
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

	server.on("/RfidCheck", HTTP_GET, [](AsyncWebServerRequest *request) {
		request->send_P(200, "text/plain", getIrTest().c_str());
		Serial.println("triggered");
		
	});
	server.on("/PasscodeCheck", HTTP_GET, [](AsyncWebServerRequest *request) {
		request->send_P(200, "text/plain", getIrTest().c_str());
	});
	
	server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest *request) {
		request->send_P(200, "text/plain", getPasscode().c_str());
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
	if(digitalRead(irTest) == 1){
		getPasscode();
	}
	
}