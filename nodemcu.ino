#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include "ThingSpeak.h"


const char* ssid = "Agro";   // your network SSID (name) 
const char* password = "0C49F477";   // your network password

WiFiClient  client;

unsigned long myChannelNumber = 1675206;
const char * myWriteAPIKey = "CS0WKYO40PKQFWEC";

// Timer variables
unsigned long lastTime = 0;
unsigned long timerDelay = 3000;

int datac1 ;
int datac2 ;
int datac3 ;
int datac4 ;


void setup() {
  Serial.begin(9600);
  while (!Serial) continue;

  WiFi.mode(WIFI_STA);   
  
  ThingSpeak.begin(client);  // Initialize ThingSpeak
}

void loop() {
  if ((millis() - lastTime) > timerDelay) {
    
    // Connect or reconnect to WiFi
    if(WiFi.status() != WL_CONNECTED){
      Serial.print("Attempting to connect");
      while(WiFi.status() != WL_CONNECTED){
        WiFi.begin(ssid, password); 
        delay(500);     
      } 
      Serial.println("\nConnected.");
    }


    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, Serial);
    if (error) return;
    datac1=doc["temp"];
    datac2=doc["hum"];
    datac3=doc["sm1"];
    datac4=doc["sm2"];
    

    ThingSpeak.setField(1,datac1);
    ThingSpeak.setField(2,datac2);
    ThingSpeak.setField(3,datac3);
    ThingSpeak.setField(4,datac4);
    ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
    
    lastTime = millis();
  }
}
