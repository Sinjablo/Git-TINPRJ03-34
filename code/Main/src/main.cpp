#include <arduino.h>
#include <stdio.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <HTTPClient.h>
#include "WiFi.h"
#include <SPIFFS.h>	//accessing flash storage on esp
#include <FS.h>		//accessing flash storage on esp
#include <Wire.h>
#include <iostream>
#include <string>
#include <keypad.h>
#include <SPI.h>	//RFID reader
#include <MFRC522.h>//RFID reader
#include <iterator>
#include <map>
#include "Adafruit_Thermal.h"	//printer
#include "SoftwareSerial.h"		//printer
#include <WiFiUdp.h>	// NTP	
#include <NTPClient.h>	// NTP
#include "time.h"	//NTP

using namespace std; 
//Wifi passwords for the ESP
// Replace with your network credentials

const char *ssid = "Tesla IoT";
const char *password = "fsL6HgjN";

//Access point credentials

const char* host = "http://145.24.222.170"; //IPv4 adress hosting the server
String get_host = "http://145.24.222.170"; //same as above

String key = "de3w2jbn7eif1nw9e";	// key used for accessing the database

//--------------- Setup keypad---------------
const byte ROWS = 4; 
const byte COLS = 4; 
char hexaKeys[ROWS][COLS] = {
  {'1', '4', '*', '7'},
  {'2', '5', '0', '8'},
  {'3', '6', '#', '9'},
  {'A', 'B', 'D', 'C'}
};
byte colPins[COLS] = {16, 4, 13, 0};	// pins corresponding to the keypad colums
byte rowPins[ROWS] = {22, 3, 5, 17}; 	//pins corresponding to the keypad rows
Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 
//----------End keypad setUp

//----------Setup RFID reader-------
const int RST_PIN = 22; // Reset pin
const int SS_PIN = 21; // Slave select pin
 
MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance

//-------------end rfid setup--------------

//-------------setup printer------------

#define TX_PIN 14  //RX op printer (blauwe kabel(6))
#define RX_PIN 1  //TX op printer (groene kabel(5))
  
SoftwareSerial mySerial(RX_PIN, TX_PIN);
Adafruit_Thermal printer(&mySerial);
//----------------end printer setup---------------

//---------setup NTP---------
//offset for NTP time keeping
const long utcOffsetInSeconds = 7200;
const char* PARAM_INPUT_1 = "input1";
// // Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);
//---------------end NTP setup---------------

AsyncWebServer server(80);	// Create AsyncWebServer object on port 80


//Variables
int page = 1;	// keeps track of what page the user is on
int tempPage;	// stores the last page the user was on
int withdrawStep = 0;	// keeps track of what step the user is in the withdrawl process
String iban;	// account number
String uid;		// UID of the bank card
String passcode;	// the users passcode
long sessionTime = 0;	//how long the sessions has been going without switching pages
String balance;		//the balance of the user(String form)
int balanceInt;		//balance of user (int form)
int generalTimeOut = 30000;	//when the program kicks the user for inactivity (30secs)
int noteArray[9];	// array with amount of bills, and the value of what the user wants to withdraw. 0 = total value, 1 = amount of bills of value thats on the next place.
int customNoteArray01[9];	//arrays with the amount and value of bills which the user can choose from to withdraw
int customNoteArray02[9];
int customNoteArray03[9];
int billsAvailable[4];	// how many of each type of bill is in the machine 0 = 5, 1 = 10, 2 = 20, 3 = 50
String billCombinationString[3];	// a string array containing the combination of bills for the user to choose from, these are send to the GUI
String leftTabs[4] = {	//array with the display option for the gui
		"1| ",
		"4| ",
		"7| ",
		"*| "
	};
boolean wrongInput = false;	//boolean that's true if the input of the user was wrong, forcing them to acknolage it.
String currency = "&euro;";	// the html code for the euro symbol
String formattedDate;	// date in: y:m:d:h:m:s
boolean resetESP = false;	// when true & page == 1, ESP is reset
boolean balanceCheck = false;	// keep track if the balance has been requested from the database
String atmUser = "ATM001";	// username of ATM
String atmPass = "ILoveMinder";	//password of ATM
String country = "";	//the country to which the bankcard belongs
String bank = "";	//the bank to which the bankcard belongs
boolean foreignBank = false;	//is true when the bankcard doesn't belong to us

//Motoren geld dispenser
const int motorForward5 = 2;
const int motorBackward5 = 12;
const int motorForward10 = 15;
const int motorBackward10 = 25;
const int motorForward20 = 26;
const int motorBackward20 = 27;
const int motorForward50 = 32;
const int motorBackward50 = 33;

// variables to return to the GUI website
char navigationKey;		//the key pressed on the keypad, which tells the gui what to do
bool abortCheck = false;	// tells gui to go back to page 1, and resets all the stored data
bool rfidCheck = false;	//check if a pass has been scanend
char loginCommand = '0';	// command for the passcode page in gui	
String menuCommand = "";	//command for the menu page in the gui
String customWithdrawOption;	//commands for the withdrawl menus in the gui
String passcodeLenght;	//the lenght that of the passcode that the user has typed
String customAmount = "";	//the amount of momey the user has selected
int CustomBillCombinationReturned = 0;	// used to cycle between the diffrent bill combinations that have to be send to the gui

std::map<String, int> fCodesMap{	// map that translated the F-codes(status codes) to int for the login page
	{"1", 42},	//pincode correct
	{"F0", 0},	// pas blocked
	{"F1", 1},	//pincode incorrect
	{"F5", 5},	//target bank not reacting
	{"F6", 6},	//target bank is unknown by country server
	{"F7", 7}	//card is unknown by target bank
};

std::map<int, int> billsAvailableIndex{	//map that has the bill type with a number
	{5, 0},
	{10, 1},
	{20, 2},
	{50, 3}
};

#pragma region //functions to access the php files that access the database/ connects to the country server

// used to verify the bankcard and pincode

String verifieer_pincode(String passcode, String accountNumber){
    
    //WiFiClient client = server.available();
		
    HTTPClient http;
    String url = get_host+"/verificatie.php?"+"sltl="+key+"&mgrkn="+accountNumber+"&mgpc="+passcode;	//constructs URL
    Serial.println(url);
    
    http.begin(url);	//accesses URL
    
    //GET method
    int httpCode = http.GET();
    String payload = http.getString();	//puts the return value of the URL in a String
	Serial.print("payload: ");
    Serial.println(payload);
    http.end(); 	//ends the http session
	return payload;
  
}

String verifieer_pincodeLand(String passcode, String accountNumber){
    
    //WiFiClient client = server.available();
		
    HTTPClient http;
    String url = get_host+"/verificatieLand.php?"+"sltl="+key+"&mgrkn="+accountNumber+"&mgpc="+passcode+"&mgld="+country+"&mgbk="+bank;
	http://145.24.222.170/verificatieLand.php?sltl=de3w2jbn7eif1nw9e&mgrkn=KC21BOEB06102001&mgpc=5467&mgld=KC&mgbk=BOEB
    Serial.println(url);
    
    http.begin(url);
    
    //GET method
    int httpCode = http.GET();
    String payload = http.getString();
	Serial.print("payload: ");
    Serial.println(payload);
    http.end(); 
	return payload;
  
}

//used to get the balance of the user

String getBalans(){
    
    //WiFiClient client = server.available();
    HTTPClient http;
	Serial.print("pincode: ");
	Serial.println(passcode);
    String url = get_host+"/balansS.php?"+"sltl="+key+"&mgrkn="+iban+"&mgpc="+passcode;
    Serial.println(url);
    
    http.begin(url);
    
    //GET method
    int httpCode = http.GET();
    String balance = http.getString();
	Serial.print("Balance: ");
    Serial.println(balance);
    http.end(); 
	balanceInt = atoi(((String)balance).c_str());
	return balance;
  
}

String getBalansLand(){
    
    //WiFiClient client = server.available();
    HTTPClient http;
	Serial.print("pincode: ");
	Serial.println(passcode);
    String url = get_host+"/balansLand.php?"+"sltl="+key+"&mgrkn="+iban+"&mgpc="+passcode+"&mgld="+country+"&mgbk="+bank;
    Serial.println(url);
    
    http.begin(url);
    
    //GET method
    int httpCode = http.GET();
    String balance = http.getString();
	Serial.print("Balance: ");
    Serial.println(balance);
    http.end(); 
	balanceInt = atoi(((String)balance).c_str());
	return balance;
  
}

//used to withdraw the money from the users account

String geldOpnemen(){
    
    //WiFiClient client = server.available();
    HTTPClient http;
	Serial.print("pincode: ");
	Serial.println(passcode);
    String url = get_host+"/opnemenS.php?"+"sltl="+key+"&mgrkn="+iban+"&mgpc="+passcode+"&bdg="+noteArray[0];
    Serial.println(url);
    
    http.begin(url);
    
    //GET method
    int httpCode = http.GET();
    String balance = http.getString();
    Serial.println(balance);
    http.end(); 
	return balance;
  
}

String geldOpnemenLand(){
    
    //WiFiClient client = server.available();
    HTTPClient http;
	Serial.print("pincode: ");
	Serial.println(passcode);
    String url = get_host+"/opnemenSLand.php?"+"sltl="+key+"&mgrkn="+iban+"&mgpc="+passcode+"&bdg="+noteArray[0]+"&mgld="+country+"&mgbk="+bank;
 
    Serial.println(url);
    
    http.begin(url);
    
    //GET method
    int httpCode = http.GET();
    String balance = http.getString();
    Serial.println(balance);
    http.end(); 
	return balance;
  
}

// used to lower the amount of bills in the ATM, that number is stored in the database
String uitStroom(String noteWorth, String noteAmount, String medewerker, String medewerkerWachtwoord){
	HTTPClient http;
	String url = get_host+"/uitstroom.php?"+"sltl="+key+"&brfj="+noteWorth+"&hvhbrfj="+noteAmount+"&mdw="+medewerker+"&mdwww="+"ILoveMinder";
	Serial.println(url);

	http.begin(url);
    
    //GET method
    int httpCode = http.GET();
    String nrOfNotes = http.getString();
    Serial.println(nrOfNotes);
    http.end(); 
	return nrOfNotes;
}

//used to get the number of bills of a certain type in the ATM
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

#pragma endregion

#pragma region 	// String return functions for the GUI, returns a value on being called.

String getAbortCheck(){		// used to end the session
	bool tempAbortCheck = abortCheck;
	abortCheck = false;
	if(tempAbortCheck == true){	// when true, ends the session and empties the variables
		page = 1;
		passcode = "";
		iban = "";
		uid = "";
		balance = "";
		withdrawStep = 0;
		customAmount = "";
		rfidCheck = false;
		resetESP = true;
		country = "";
		bank = "";
		Serial.println("Abort has been called");
	}
	return String(tempAbortCheck);
}
String getNavigation(){		// returns a number in string form that corresponds to a keypad press
	
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
String getRfidCheck(){		// tells gui that the keypad has read a card (page=1)
	bool tempRfidCheck = rfidCheck;
	rfidCheck = false;
	return String(tempRfidCheck);
	
}
String getLoginCommand(){	//returns command for pincode page (page=2)
	char tempLoginCommand = loginCommand;
	loginCommand = '0';
	return String(tempLoginCommand);
}
String getPasscodeLenght(){	//returns the lenght of what the user has type for their password (page 2)
	return String(passcode.length());
}
String getMenuCommand(){	//returns commands for the main menu
	String returnString;
	if(menuCommand != ""){
		returnString = menuCommand;
		menuCommand = "";
	}
	return String(returnString);
}
String getAccountNumber(){	//returns the account number (page=7)
	return String(iban);
}
String getBalance(){	//returns the balance of the use
	if(foreignBank){
		return String(getBalansLand());
	}else{
		return String(getBalans());
	}
}
String getCustomAmount(){	//returns the amount the user has selected for withdrawl, can also be used to display messages
	return String(customAmount);
}
String getWithdrawOption(){	//returns commands for the withdrawl menus
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
	if(customWithdrawOption != ""){
		Serial.println("customWithdrawOption in get menu");
		returnString = customWithdrawOption;
		customWithdrawOption = "";
	}
	return String(returnString);
}

#pragma endregion


void setup(){	// void setup
	// Serial port for debugging purposes
	Serial.begin(115200);
	//printer setup
	mySerial.begin(9600);	// baud rate for the printer
    printer.begin();	//starts the printer

	//motors
	pinMode(motorForward5, OUTPUT);
	pinMode(motorBackward5, OUTPUT);
  	pinMode(motorForward10, OUTPUT);
  	pinMode(motorBackward10, OUTPUT);
  	pinMode(motorForward20, OUTPUT);
  	pinMode(motorBackward20, OUTPUT);
  	pinMode(motorForward50, OUTPUT);
  	pinMode(motorBackward50, OUTPUT);

	//motors reset
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

	//initialize the html web pages
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
	// route to abort the transaction processes and return to index
	server.on("/abort", HTTP_GET, [](AsyncWebServerRequest *request) {
		request->send_P(200, "text/plain", getAbortCheck().c_str());
	});
	// route to check if RFID has been scanned
	server.on("/rfidCheck", HTTP_GET, [](AsyncWebServerRequest *request) {
		request->send_P(200, "text/plain", getRfidCheck().c_str());		
		page = 1;
	});
	// route for the login command
	server.on("/loginCommand", HTTP_GET, [](AsyncWebServerRequest *request) {
		request->send_P(200, "text/plain", getLoginCommand().c_str());
		page = 2;
	});
	// route to get the passcode lenght
	server.on("/passcodeLenght", HTTP_GET, [](AsyncWebServerRequest *request) {
		request->send_P(200, "text/plain", getPasscodeLenght().c_str());
	});
	// route to get the keypad input general use
	server.on("/navigation", HTTP_GET, [](AsyncWebServerRequest *request) {
		request->send_P(200, "text/plain", getNavigation().c_str());
		page = 3;
	});
	// route to get mainmenu command
	server.on("/menuCommand", HTTP_GET, [](AsyncWebServerRequest *request) {
		request->send_P(200, "text/plain", getMenuCommand().c_str());
	});
	// route to get the keypad input for withdraw menu
	server.on("/withdrawlNav", HTTP_GET, [](AsyncWebServerRequest *request) {
		request->send_P(200, "text/plain", getNavigation().c_str());
		page = 4;
	});
	server.on("/customAmount", HTTP_GET, [](AsyncWebServerRequest *request) {
		request->send_P(200, "text/plain", getCustomAmount().c_str());
	});
	// route to get the keypad input for custom withdraw menu
	server.on("/customWithdrawlNav", HTTP_GET, [](AsyncWebServerRequest *request) {
		request->send_P(200, "text/plain", getNavigation().c_str());
		page = 5;
	});
	// route to get custom withdraw menu commands
	server.on("/customWithdrawOptions", HTTP_GET, [](AsyncWebServerRequest *request) {
		request->send_P(200, "text/plain", getWithdrawOption().c_str());
	});
	// route to get the keypad input for receipt menu
	server.on("/receiptNav", HTTP_GET, [](AsyncWebServerRequest *request) {
		request->send_P(200, "text/plain", getNavigation().c_str());
		page = 6;
	});
	// route to get the keypad input for the balance menu
	server.on("/balanceNavigation", HTTP_GET, [](AsyncWebServerRequest *request) {
		request->send_P(200, "text/plain", getNavigation().c_str());
		page = 7;
	});
	// route to get the balance of the user
	server.on("/getBalance", HTTP_GET, [](AsyncWebServerRequest *request) {
		request->send_P(200, "text/plain", getBalance().c_str());
	});
	// route to get the account number of the user
	server.on("/accountNumber", HTTP_GET, [](AsyncWebServerRequest *request) {
		request->send_P(200, "text/plain", getAccountNumber().c_str());
	});

	server.begin();// Starts server

	//initialize RFID reader
	SPI.begin(); // Init SPI bus
	mfrc522.PCD_Init(); // Init MFRC522
	mfrc522.PCD_DumpVersionToSerial(); // Show details of PCD - MFRC522 Card Reader details

	timeClient.begin();//starts NTP
}

#pragma region  // Functions for the diffrent pages

void ibanDeconstructor(){	//deconstructs the IBAN to get the country and bank
	for(int i = 0; i < 2; i++){
		country += iban.charAt(i);	// gets the country
	}
	for(int i = 4; i < 8; i++){
		bank += iban.charAt(i);	//gets the bank
	}
	if((country+bank) != "KCBOEB"){		//checks if the card belongs to us or another bank
		Serial.println("passholder's bank is a foreign bank");
		foreignBank = true;
	}
	Serial.print("Pass Country: ");
	Serial.println(country);
	Serial.print("Pass bank: ");
	Serial.println(bank);
}

void rfidReader(){		//reads the RFID card at the scanner
	
	
	
	// checks if the card can be read
	if (!mfrc522.PICC_ReadCardSerial()){
		return;
	}
	// empties the Iban
	iban = "";
    // Prepare key - all keys are set to FFFFFFFFFFFFh at chip delivery from the factory.
  	MFRC522::MIFARE_Key key;
  	for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;
  
  	MFRC522::StatusCode status;

	byte block = 1;
  	byte len;
  	byte readBuffer[18];

	// checks for the status of authentication
	status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 1, &key, &(mfrc522.uid));
  	if (status != MFRC522::STATUS_OK) {
  	  Serial.print(F("Authentication failed: "));
  	  Serial.println(mfrc522.GetStatusCodeName(status));
  	  return;
  	}
	//checks for status of the reading of the card
  	status = mfrc522.MIFARE_Read(block, readBuffer, &len);
  	if (status != MFRC522::STATUS_OK) {
  	  Serial.print(F("Reading failed: "));
  	  Serial.println(mfrc522.GetStatusCodeName(status));
  	  return;
  	}
	//constructs the iban into a string from the card
  	Serial.print("Rekening Nummer: ");
  	for (uint8_t i = 0; i < 16; i++){
  	  iban += (char)readBuffer[i];
  	}

	mfrc522.PICC_HaltA();
  	mfrc522.PCD_StopCrypto1();
	
  	Serial.println(iban);
	
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
	ibanDeconstructor();	//calls to deconstruct the Iban
}

void passcodeChecker(char customKey){	//checks the passcode & iban of the user
	// check for input, check if passcode is 4 digits, check if 'A' has been pressed, check if password is correct, send lenght of passcode to passcodeLenght


	if (customKey == 'A' || customKey == 'B' || customKey == 'C' || customKey == 'D' || customKey == '*' || customKey == '#'){
		loginCommand = customKey;
		if (customKey == 'A' && passcode.length() == 4){	//makes sure the type passcode is 4 characters
			int fCode;	//temporary int to store the F-code
			//get the f-code, and gets the corresponding int back from a map
			if(foreignBank){	// checks if the bankcard is ours
				// verificatie land
				//get the F-code for foreign banks
				fCode = fCodesMap[verifieer_pincodeLand(passcode, iban)];
			}else{
				fCode = fCodesMap[verifieer_pincode(passcode, iban)];	// gets the F-code for our bank
			}
			switch (fCode){		//switches to the correct option based on the f-code given
				case 42:	// login succes
					loginCommand = '1';
					break;
				case 0:		//pincode wrong
					passcode = "";
					loginCommand = '2';
					break;
				case 1:		// pas blocked
					passcode = "";
					loginCommand = '3';
					break;
				case 5:		//target bank not reacting
					loginCommand = '4';
					break;
				case 6:		//target bank is unknown by country server
					loginCommand = '5';
					break;
				case 7:		//card is unknown by target bank
					loginCommand = '6';
					break;
			}
			
		}
		else if (customKey == 'B'){	//empties types passcode
			passcode = "";
		}
	}
	else if (passcode.length() < 4){	//makes sure types passcode is not longer than 4 characters
		passcode += customKey;
		Serial.println(passcode);
	}
}

void navigationInput(char customKey){	//sets the pressed key as navifationKey for the gui
	navigationKey = customKey;
	Serial.print("navKey: ");
	Serial.println(navigationKey);
}

void dispense50(int value) {	//dispenses a 50 euro bill
  for (value; value != 0; value--) {
    Serial.println("50 word uitgeworpen");
    digitalWrite(motorForward50, HIGH);
    delay(3300);
    digitalWrite(motorForward50, LOW);
    digitalWrite(motorBackward50, HIGH);
    delay(1300);
    digitalWrite(motorBackward50, LOW);
	uitStroom("50", "1", atmUser, atmPass);
  }
}
void dispense20(int value) {	//dispenses a 20 euro bill
  for (value; value != 0; value--) {
    Serial.println("20 word uitgeworpen");
    digitalWrite(motorForward20, HIGH);
    delay(1500);
    digitalWrite(motorForward20, LOW);
    digitalWrite(motorBackward20, HIGH);
    delay(800);
    digitalWrite(motorBackward20, LOW);
	uitStroom("20", "1", atmUser, atmPass);
  }
}
void dispense10(int value) {	//dispenses a 10 euro bill
  for (value; value != 0; value--) {
    Serial.println("10 word uitgeworpen");
    digitalWrite(motorForward10, HIGH);
    delay(1500);
    digitalWrite(motorForward10, LOW);
    digitalWrite(motorBackward10, HIGH);
    delay(800);
    digitalWrite(motorBackward10, LOW); 
	uitStroom("10", "1", atmUser, atmPass);
  }
}
void dispense5(int value) {		//dispenses a 5 euro bill
  for (value; value != 0; value--) {
    Serial.println("5 word uitgeworpen");
    digitalWrite(motorForward5, HIGH);
    delay(1300);
    digitalWrite(motorForward5, LOW);
    digitalWrite(motorBackward5, HIGH);
    delay(600);
    digitalWrite(motorBackward5, LOW);
	uitStroom("5", "1", atmUser, atmPass);
  }
}

void dispenseMoney(){	// dispenses the bills selected by the user
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
	abortCheck = true;	//ends the session
}

void billsInMachine(){	// retrieves the bills currenty in the machine
	String bills[4] = {"5", "10", "20", "50"};
	for(int i = 0; i < 4; i++){
		billsAvailable[i] = aantalBriefjes(bills[i], atmUser, atmPass);	// puts the number of available bills in an arry
	}
}

void billConstructor(int customAmountInt, String customAmountStr, int customAmountLenght){	//puts the selected amount through 3 algorithems that generete 3 options for combinations of bills to be withdrawn
	Serial.println("billConstructor");
	int customAmountMaster = customAmountInt;	// a copy of the amount of money chosen, is used to make copies of later to reference the total amount left
	int customAmountConstructor = customAmountMaster;
	//keeps track of where we are in the constructed string array
	int locationArray01 = 1;
	int locationArray02 = 1;
	int locationArray03 = 1;
	//clears the arrays
	for(int i = 0; i < 9; i ++){
		customNoteArray01[i] = 0;
	}
	for(int i = 0; i < 9; i ++){
		customNoteArray02[i] = 0;
	}
	for(int i = 0; i < 9; i ++){
		customNoteArray03[i] = 0;
	}
	//copies the total amount selected to spot 0 in the array
	customNoteArray01[0] = customAmountConstructor;
	customNoteArray02[0] = customAmountConstructor;
	customNoteArray03[0] = customAmountConstructor;
	int divideCalculation;	// int to store calculations in.
	boolean endsOn5 = false;	// boolean is true when custom amount ends on 5
	int billOptions[4] = {50, 20, 10, 5};	// the diffrent bills which can be selected
	if(customAmountStr.charAt(customAmountLenght) == '5'){		// checks if the custom amount ends on a 5, so yes, removes the 5 and adds it to the array
		customNoteArray01[locationArray01] = 1;
		customNoteArray01[locationArray01 + 1] = 5;
		customNoteArray02[locationArray02] = 1;
		customNoteArray02[locationArray02 + 1] = 5;
		customNoteArray03[locationArray03] = 1;
		customNoteArray03[locationArray03 + 1] = 5;
		customAmountMaster -= 5;	//renm
		locationArray01 += 2;
		locationArray02 += 2;
		locationArray03 += 2;
		endsOn5 = true;
	}

	if(customAmountMaster != 0){	// makes sure the amount left isn't 0
		#pragma region	//algorithem 1
		// constructs the first array with bills, divides the amount up into the least amount of bills
		customAmountConstructor = customAmountMaster;
		for(int i = 0; i < 3; i++){		
			divideCalculation = customAmountConstructor / billOptions[i];
			if(divideCalculation >= 1){
				Serial.println(billOptions[i]+" divide");
				customNoteArray01[locationArray01] = divideCalculation;
				customNoteArray01[locationArray01 + 1] = billOptions[i];
				customAmountConstructor -= billOptions[i] * divideCalculation;
				locationArray01 += 2;
			}
		}
		#pragma endregion 
		#pragma region	//algorithem 2
		// constructs the second array with bills. works in 2 steps:
		// step 1: divides the amount, and gives the first half in as little as possible notes
		// step 2: gives the second half completely in the notes that are next in line to the biggest note from step 1, if we get stuck -> onto the next note.
		Serial.println("algo02");
		customAmountConstructor = customAmountMaster;
		int half1 = customAmountConstructor / 2;
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
		// constructs the fourth array with bills, 
		//starts with notes of 10, if the total number exeeds 10, switches to notes of
		customAmountConstructor = customAmountMaster;
		while(customAmountConstructor != 0){
			for(int i = 2; i > -1; i--){
				divideCalculation = customAmountConstructor / billOptions[i];
				if(divideCalculation >= 1){
					if(divideCalculation < 11 || billOptions[i] == 50){
						customNoteArray03[locationArray03] = divideCalculation;
						customNoteArray03[locationArray03 + 1] = billOptions[i];
						locationArray03 += 2;
						customAmountConstructor -= divideCalculation * billOptions[i];
					}
				}
			}
		}
		#pragma endregion

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
	//prints the results of the 3 algorithems
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
}

void billArrayStringConstructor(int billArray[9], int arrayNumber){	//constructs strings useable for the gui from the arrays made in billConstructor()
	int increament = 2;
	if(billArray[0] == 0){	//checks if the array has been declared unuseable
		billCombinationString[arrayNumber] = "";	//makes the string empty
	}else{
		for(int u = 1; u < 9; u += increament){	//constructs the String from the array
			if(billArray[u] == 0){// checks for the end of the array
				break;
			}
			if(billCombinationString[arrayNumber] == ""){	//if the string is still empty, first give it the correct position for the gui
				billCombinationString[arrayNumber] += leftTabs[arrayNumber];
			}
			String billAmount = String(billArray[u]);
			String value = String(billArray[u+1]);
			billCombinationString[arrayNumber] += billAmount+" x "+currency+value+" ";
		}	
	}

}

void billArrayChecker(int billArray[9], int arrayNumber){	// function to check if the bills selected are available.

	int increament = 2;	// increament of 2 to skip a space in the array for each loop
	Serial.println(billArray[0]);
	for(int i = 1; i < 9; i += increament){
		if(billArray[i] == 0){	// checks for the end of the array
			break;
		}else if(billsAvailable[billsAvailableIndex[billArray[i+1]]] < billArray[i]){	//checks if the amount of notes selected are available in the machine
			Serial.print("Er zijn te weinig briefjes van: ");
			Serial.println(billArray[i+1]);
			billArray[0] = 0;	// puts the 0 position of the array to 0, marking it unusable. Used in billArrayStringConstructor()
			break;
		}
	}
	Serial.println(billArray[0]);
	billArrayStringConstructor(billArray, arrayNumber);
	return;
}

void withdrawlMenu(char customKey){		//withdraw menu, user can select one of three preselected values to withdraw. the user can also switch to a menu to enter the value themselfs
	Serial.print("customKey: ");
	Serial.println(customKey);
	Serial.print("Withdraw step: ");
	Serial.println(withdrawStep);
	if(customKey == '1' || customKey == '4' || customKey == '7' || customKey == '*'){	//keypad presses for selection of pre-determaind values
		if(withdrawStep == 0 || withdrawStep == 1){
			navigationKey = customKey;
			// switches based on the keypress, 1,4,&7 are for amount selection, * is for the custom withdraw menu 
			switch (customKey){
				case '1':
					if(balanceInt >= 10){	//checks if the users balance is high enough
						customWithdrawOption = "u0000"; //clears the display message;
						if(withdrawStep == 0){
							billsInMachine();	//gets the amount of bills in the machine
							billConstructor(10, "10", 1);	// constructs 3 arrays with driffrent bill combinations
						}else{
							Serial.println("array: 1");
							for(int i = 0; i < 9; i++){	//transfers the selected array to the main array used for dispensing money
								noteArray[i] = customNoteArray01[i];
								Serial.print(noteArray[i]);
							}
							Serial.println("");
							customAmount = billCombinationString[0].substring(2, billCombinationString[0].length());	// sets the custom amount to the constructed string of the selected array
						}
					}else{
						customWithdrawOption = "i0000"; //tells the user that their balance is insuffisiant
						return;
					}
					break;
				case '4':
					if(balanceInt >= 30){
						customWithdrawOption = "u0000"; //clears the display message;
						if(withdrawStep == 0){
							billsInMachine();
							billConstructor(30, "30", 1);
						}else{		
							Serial.println("array: 2");
							for(int i = 0; i < 9; i++){
								noteArray[i] = customNoteArray02[i];
								Serial.print(noteArray[i]);
							}
							Serial.println("");
							customAmount = billCombinationString[1].substring(2, billCombinationString[1].length());
						}
					}else{
						customWithdrawOption = "i0000"; //tells the user that their balance is insuffisiant
						return;
					}
					break;
				case '7':
					if(balanceInt >= 70){
						customWithdrawOption = "u0000"; //clears the display message;
						if(withdrawStep == 0){
							billsInMachine();
							billConstructor(70, "70", 1);
						}else{						
							Serial.println("array: 3");
							for(int i = 0; i < 9; i++){
								noteArray[i] = customNoteArray03[i];
								Serial.print(noteArray[i]);
							}
							Serial.println("");
							customAmount = billCombinationString[2].substring(2, billCombinationString[2].length());
						}
					}else{
						customWithdrawOption = "i0000"; //tells the user that their balance is insuffisiant
						return;
					}
					break;
				case '*':
					if(withdrawStep == 0){
						withdrawStep = 0;
						customAmount = "";
						customWithdrawOption = "c0000";	//tells gui to switch to custom withdraw menu
						return;
					}
					break;
			}
			if(withdrawStep == 0){
				// checks if the notes in the constructed array are available
				billArrayChecker(customNoteArray01, 0);
				billArrayChecker(customNoteArray02, 1);
				billArrayChecker(customNoteArray03, 2);
				withdrawStep = 1;
			}else{
				if(customKey == '1' || customKey == '4' || customKey == '7'){
					for(int i = 0; i < 3; i++){	// fills the string arrays for the gui with their stock message
						billCombinationString[i] = leftTabs[i];
					}
					customWithdrawOption = "a0000";	// empties the left tabs on the gui
					withdrawStep = 2;
				}
			}

		}
	}else if(customKey == 'A' || customKey == 'B' || customKey == 'C' || customKey == 'D'){	//keypresses for ok, return & cancel
		navigationKey = customKey;
		if(customKey == 'A' && withdrawStep == 2){	//for when the selection process is done
			customWithdrawOption = "d0000";	// tells the gui to switch to the receipt menu
		}
		if(customKey == 'B' || customKey == 'C'){	//empties the selection
			customWithdrawOption = "q0000";	// tells gui to reset
			customAmount = "";
			withdrawStep = 0;
			for(int i = 0; i < 3; i++){
				billCombinationString[i] = leftTabs[i];
			}
		}
	}
}

void customAmountMenu(char customKey){	//the user can enter a custom value of what they want to withdraw

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
				customWithdrawOption = "d";	// tells the gui to switch to the receipt menu
			}
			if(wrongInput == true){
				Serial.println("wrong input yeye");
				customAmount = "";
				customWithdrawOption = "u";
				wrongInput = false;
				return;
			}
			if(customAmount.charAt(customAmountLenght) == '5' || customAmount.charAt(customAmountLenght) == '0'){
				int customAmountInt = atoi(((String)customAmount).c_str());
				if(balanceInt >= customAmountInt){	// problem is probs customAmountInt
					if(customAmount.charAt(customAmountLenght) == '5' && aantalBriefjes("5", atmUser, atmPass) == 0){
						customAmount = "Helaas zijn de 5$ biljetten momenteel op, probeer later opnieuw.";
						wrongInput = true;
					}else if(withdrawStep == 0){
						Serial.print("customAmountInt: ");
						Serial.println(customAmountInt);
						Serial.print("customAmountLenght: ");
						Serial.println(customAmountLenght);
						billsInMachine();
						billConstructor(customAmountInt, customAmount, customAmountLenght);
						billArrayChecker(customNoteArray01, 0);
						billArrayChecker(customNoteArray02, 1);
						billArrayChecker(customNoteArray03, 2);
						withdrawStep = 1;
					}
				}else{
					Serial.println("loc0");
					customWithdrawOption = "i";	//tells the user that their balance is insuffisiant
					customAmount = "";
					wrongInput = true;
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
					customWithdrawOption = "q";	// resets the display message to the original
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
					customAmount = billCombinationString[1].substring(2, billCombinationString[1].length());
					break;
				case '7':
					Serial.println("array: 3");
					for(int i = 0; i < 9; i++){
						noteArray[i] = customNoteArray03[i];
						Serial.print(noteArray[i]);
					}
					Serial.println("");
					customAmount = billCombinationString[2].substring(2, billCombinationString[2].length());
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

void updateTime(){	//updates the date and time for the receipt
	timeClient.update();

	formattedDate = timeClient.getFullFormattedTime();	// puts the date & time into a String
	Serial.print("Date: ");
	Serial.println(formattedDate);
}

void receiptPrinter(){	//prints the receipt
	updateTime();// updates the time

	printer.println();
    printer.println();
    
    //name of our bank
    printer.justify('C');
    printer.setSize('L');
    printer.print(F("BOEJIEBANK\n"));  
    printer.justify('L');
    printer.setSize('S');

    printer.println();
    printer.println();
 
   //prints the date & time
    printer.print(F("Datum : "));
	printer.print(formattedDate);
	printer.println();

   // prints the last 4 digits of the Iban
    String passNumber = "************";
	for(int i = 12; i < 16; i ++){
		passNumber += iban.charAt(i);
	}
	Serial.println(passNumber);
    printer.println(passNumber);
   	printer.println();
  
   //prints the amount withdrawn
    printer.setSize('M');
	printer.print("Opgenomen bedrag: ");
    //printer.write(20AC); printer.print("100 ");
    printer.print(String(noteArray[0]));

    printer.println();
    printer.println();
    
    //prints goodbye message
    printer.justify('C');
    printer.setSize('M');
    printer.println(F("Tot ziens & fijne dag"));
    //printer.println(F("bij BOEJIEBANK!\n"));
    printer.println();
    printer.println();
    printer.println();
	
}

void receiptMenu(char customKey){	// gets called when the user is on the receipt menu
	switch (customKey){	// switches based on if the user want a receipt or not
		case 'A':	//wants a receipt
			receiptPrinter();	//prints a receipt
			geldOpnemen();	//withdraws money from account
			dispenseMoney();	//dispenses money
			break;
		case 'B':	//no receipt
			geldOpnemen();	//withdraws money from account
			dispenseMoney();	// dispenses money
			break;
	}
}

void quickWithdraw(){	// function that quickly withdraws & dispenses 70 euros when called. Can be called from the main menu
	billsInMachine();
	billConstructor(70, "70", 1);
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
	switch (foreignBank){
		case 0:
			geldOpnemen();
			break;

		case 1:
			geldOpnemenLand();
			break;
	}
	
	dispenseMoney();
}

#pragma endregion

void timerReset(){	// resets the timer for time out

	if(page != tempPage){ //checks if the user has switched pages
		sessionTime = millis();	//resets the inactivity timer
	}
	tempPage = page;

}

void timerControl(){	// function to abort if the user has been inactive for too long
	
	//Serial.print("time: ");
	//Serial.println(millis()-sessionTime);
	if(millis()-sessionTime > generalTimeOut){
		abortCheck = true;
		page = 1;
	}

}

void loop(){	//void main
	if(page == 1){
		if(resetESP){
			delay(2500);
			ESP.restart();
			resetESP = false;
		}
		// Look for new cards
		if(mfrc522.PICC_IsNewCardPresent()){
			rfidReader();
			rfidCheck = true;
			sessionTime = millis();
			Serial.println("--------------------------------------------------------------loc1");
			delay(200);
		}
		
	}else{
		timerControl(); //function to start timer && check the spend time
		timerReset();	// checks if the the page has been switched and the timer can be reset.
		if(page != 2 && balanceCheck == false){
			switch (foreignBank){
				case 0:
					getBalans();
					break;

				case 1:
					getBalansLand();
					break;
			}	
			if(balanceInt < 70){	//checks if balance below 70$, remoce quick 70$ from main menu
				menuCommand = "remove70";
			}
			balanceCheck = true;
		}
		char customKey = customKeypad.getKey();
  		if (customKey){
			switch (page){
				case 2:	//--------------------take the keypad input for the passcode
					passcodeChecker(customKey);
					break;
				case 3:	//---------------- take keypad input for navigation
					navigationInput(customKey);
					if(customKey == '7' && balanceInt >= 70){
						quickWithdraw();// quickly withdraws 70 euros
					}
					break;
				case 4://-----------------withdraw menu
					withdrawlMenu(customKey);
					vTaskDelay(10);
					break;
				case 5://-------------------------custom withdraw menu
					customAmountMenu(customKey);
					break;
				case 6://-----------------receipt menu
					receiptMenu(customKey);
					break;
				case 7://----------------------balance menu
					navigationInput(customKey);
					break;
			}
			if(customKey == 'D'){
				abortCheck = true;
			}
  		}
	}
	yield();	// gives the esp32 time to catch up
}