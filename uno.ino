
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
