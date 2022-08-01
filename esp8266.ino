
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>

#ifndef STASSID
#define STASSID "SLT_4GLTE"    //access point wifi name (SSID)
#define STAPSK  "0C49F477"  //access point wifi password
#endif

const char* ssid     = STASSID;
const char* password = STAPSK;

const char* host = "api.thingspeak.com";
const uint16_t port = 443;

// SHA1 fingerprint of the certificate
const char fingerprint[] PROGMEM = "27:18:92:DD:A4:26:C3:07:09:B9:7A:E6:C5:21:B9:5B:48:F7:16:E1";
//WiFiClientSecure client;
int data1, data2, data3, data4;


void ESPSendData(int tempe, int humid , int soilm, int carbon)
{
  char buf[100];
  sprintf(buf, "GET /update?api_key=X36666UEO8K42G4I&field1=%lu&field2=%lu&field3=%lu&field4=%lu HTTP/1.1\r\nHost: api.thingspeak.com\r\n\r\n", tempe, humid, soilm, carbon);
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
