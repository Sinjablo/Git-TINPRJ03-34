//include libraries
#include <HttpClient.h>
#include <WiFi.h>
 
//Access point credentials
//const char* ssid = "VielvoyeResidence24GHz";
//const char* pwd = "Oli/5iN-dR=88#VRGHZ#24";

//const char* ssid = "LaptopieVanSander";
//const char* pwd = "KrijgsheerSander";

const char* host = "http://192.168.178.73";
String get_host = "http://192.168.178.73";

String pincode;
String rekeningnummer;
String sleutel = "de3w2jbn7eif1nw9e";
WiFiServer server(80);  // open port 80 for server connection
void setup() 
{
  Serial.begin(115200); //initialise the serial communication
  delay(20);
  WiFi.begin(ssid, pwd);
    //starting the server
    server.begin();
}
void loop()
{
  //call_test();
  verifieer_pincode("1232","NL21HAHA010032003");
       
}

void verifieer_pincode(String pincode, String rekeningnummer)
  {
    
    //WiFiClient client = server.available();
        HTTPClient http;
        String url = get_host+"/verificatie.php?"+"sltl="+sleutel+"&mgrkn="+rekeningnummer+"&mgpc="+pincode;
        Serial.println(url);
        
        http.begin(url);
       
        //GET method
        int httpCode = http.GET();
        String payload = http.getString();
        Serial.println(payload);
        http.end();
        delay(1000);
  
  }
