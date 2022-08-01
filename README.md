# Smart Agricultural System Using Arduino

Smart Agricultural System  allows to interact and improve the management of agricultural farms, this project will also allow intelligent management of hydraulic resources automatic irrigation systems, and according to the need of plants, it will also allow to control the humidity and the ambient temperature in the greenhouse, which will allow to have a global vision on what will happen in the greenhouse equipped with sensors and actuator, this will enable the intelligent management of the farm.

---

# Layouts

<img src="https://github.com/uddhikaku/SmartAgriculturalSystem/blob/main/Layout%201.png" width="25%" />

---

<img src="https://github.com/uddhikaku/SmartAgriculturalSystem/blob/main/Layout%202.png" width="25%" />

---

<img src="https://github.com/uddhikaku/SmartAgriculturalSystem/blob/main/Layout%203.png" width="25%" />

---

<img src="https://github.com/uddhikaku/SmartAgriculturalSystem/blob/main/Layout%204.png" width="25%" />

---

<img src="https://github.com/uddhikaku/SmartAgriculturalSystem/blob/main/Layout%205.png" width="25%" />

---

# Thingspeak Server SS

<img src="https://github.com/uddhikaku/SmartAgriculturalSystem/blob/main/Layout%206.png" width="50%" />


---

material list:

  1. UNO+WiFi R3 ATmega328P+ESP8266 32Mb Memory USB-TTL CH340G
  2. NodeMcu WIFI Internet Development Board ESP8266 CP2102
  3. AM2302B DHT22 Digital Temperature And Humidity Sensor AM2302
  4. Soil Moisture Sensor And Soil Detector Module
  5. DC12V 6L/Min Large Flow Rate Water Pump Micro High Pressure Diaphragm Water Sprayer
  6. LCD16x02 Display Interactive Interface single-chip Blue
  7. 5V 2 Channel Relay Module With Optocoupler LED
  8. Electric Solenoid Valve 1/2" Plastic DC 12V 0.6A N/C For Water Air
---

# Description

This project can identify Temperature, Humidity and Soil Moisture value. after reading all values it's showing using device 16x02 LCD display and after its send data to thingspeak server and calculate data and get into chart. after the data showing using mobile app and web site.
After using nodemcu board connect with firebase realtime database and using mobile app we can control water supply using mobile application.

---

# Arduino UNO Code

----


#include <ArduinoJson.h>
#include <LiquidCrystal.h>
#include "DHT.h"
#define DHTPIN 7     
#define DHTTYPE DHT22   
DHT dht(DHTPIN, DHTTYPE);


//Variables Soil Moisture
int sensorPin = A0; 
int sensorValue;
int sm;  
int limit = 500;

//Variable CO
const int AOUTpin=A1;//the AOUT pin of the CO sensor goes into analog pin A1 of the arduino
const int DOUTpin=8;//the DOUT pin of the CO sensor goes into digital pin D8 of the arduino
const int ledPin=10;//the anode of the LED connects to digital pin D13 of the arduino
int value;
int co;
int limitCO;

//Display
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);


void setup() {
  Serial.begin(9600);
  dht.begin();
  pinMode(13, OUTPUT);
  pinMode(DOUTpin, INPUT);//sets the pin as an input to the arduino
  pinMode(ledPin, OUTPUT);//sets the pin as an output of the arduino

  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("Smart Agricultural");
  lcd.setCursor(4, 1);
  lcd.print("System");
  delay(3000);

}

StaticJsonDocument<1000> doc;

void loop() {
 
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();

  // If the DHT-22 is not connected to correct pin 
  // or if it does not work, no data will be sent
  if (isnan(h) || isnan(t)) {
    return;
 }
  
  doc["temp"] = t;
  doc["hum"] = h;
    //Print temp and humidity values to serial monitor
    Serial.print("Humidity: ");
    Serial.println(h);
    Serial.print("Temperature: ");
    Serial.print(t);
    Serial.println(" Celsius");

   lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Humidity: ");
    lcd.setCursor(9,0);
    lcd.print(h);
    lcd.setCursor(14,0);
    lcd.print("%");
    lcd.setCursor(0, 1);
    lcd.print("Tempera : ");
    lcd.setCursor(9,1);
    lcd.print(t);
    lcd.setCursor(14,1);
    lcd.print("'C");
    delay(3000); //Delay 3 sec.

   sensorValue = analogRead(sensorPin);
    doc["sm"] = sensorValue; 
    Serial.print("Soil Moisture Value : ");
    Serial.println(sensorValue);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Soil Mois: ");
    lcd.setCursor(11,0);
    lcd.print(sensorValue);

   value= analogRead(AOUTpin);//reads the analaog value from the CO sensor's AOUT pin
    limitCO= digitalRead(DOUTpin);//reads the digital value from the CO sensor's DOUT pin
    Serial.print("CO value : ");
    doc["co"] = value;
    Serial.print(value);//prints the CO value
    lcd.setCursor(0,1);
    lcd.print("CO Value : ");
    lcd.setCursor(11,1);
    lcd.print(value);
    delay(3000);
    
   if (value >= 460){
      digitalWrite(13, HIGH);
    }
    else{
      digitalWrite(13, LOW);
    }
    
   if(Serial.available()>0)
    {
      serializeJson(doc, Serial);
    }
    delay(3000); // 30 detik
  }

---

# ESP8266 Code

---

#include <ArduinoJson.h>
#include <ESP8266WiFi.h>

#ifndef STASSID
#define STASSID "UDDHIKA"    //access point wifi name (SSID)
#define STAPSK  "0C49F477"  //access point wifi password
#endif

const char* ssid     = STASSID;
const char* password = STAPSK;

const char* host = "api.thingspeak.com";
const uint16_t port = 443;

// SHA1 fingerprint of the certificate
const char fingerprint[] PROGMEM = "27:18:92:DD:A4:26:C3:09:59:B9:75:E6:65:21:B9:5B:48:F7:16:E1";
//WiFiClientSecure client;
int data1, data2, data3, data4;


void ESPSendData(int tempe, int humid , int soilm, int carbon)
{
  char buf[100];
  sprintf(buf, "GET /update?api_key=X36666UEO8K45534534I&field1=%lu&field2=%lu&field3=%lu&field4=%lu HTTP/1.1\r\nHost: api.thingspeak.com\r\n\r\n", tempe, humid, soilm, carbon);
  //WiFiClient client;
  WiFiClientSecure client;
  //Serial.printf("Using fingerprint '%s'\n", fingerprint);
  client.setFingerprint(fingerprint);
  if (!client.connect("api.thingspeak.com", 443)) {
    //Serial.println("Failed to connect with 'api.thingspeak.com' !");
  }
  else {
    int timeout = millis() + 5000;
    client.print(buf);
    while (client.available() == 0) {
      if (timeout - millis() < 0) {
        //Serial.println(">>> Client Timeout !");
        client.stop();
        return;
      }
    }
    int size;
    while ((size = client.available()) > 0) {
      uint8_t* msg = (uint8_t*)malloc(size);
      size = client.read(msg, size);
      //Serial.write(msg, size);
      free(msg);
    }
  }
}

void setup() {
  Serial.begin(9600);
  while (!Serial) continue;

  // We start by connecting to a WiFi network
  //Serial.println();
  //Serial.println();
  //Serial.print("Connecting to ");
  //Serial.println(ssid);

  /* Explicitly set the ESP8266 to be a WiFi-client, otherwise, it by default,
     would try to act as both a client and an access-point and could cause
     network-issues with your other WiFi-devices on your WiFi-network. */
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    //Serial.print(".");
  }

  //Serial.println("");
  //Serial.println("WiFi connected");
  //Serial.println("IP address: ");
  //Serial.println(WiFi.localIP());
}

void loop() {
  StaticJsonDocument<1000> doc;  
  DeserializationError error = deserializeJson(doc, Serial);
  if (error) return;
  data1=doc["temp"];
  data2=doc["hum"];
  data3=doc["sm"];
  data4=doc["co"];
  Serial.print(data1);

  if (data1>0 && data2>0 && data3>0 && data4>0) {
    ESPSendData(data1, data2, data3, data4);  
  }
  
  delay(3000); // 60 detik
}


---

# Nodemcu Code

---

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

---

