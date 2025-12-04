#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Wire.h>
#include <BH1750.h>
#include "DHT.h"

const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
String apiKey = "YOUR_THINGSPEAK_API_KEY";
String server = "http://api.thingspeak.com/update";

#define DHTPIN D4         
#define DHTTYPE DHT22
#define SOIL_PIN A0       
#define RELAY_PIN D1       

DHT dht(DHTPIN, DHTTYPE);
BH1750 lightMeter;

void setup() {
  Serial.begin(115200);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);

  dht.begin();
  Wire.begin(D2, D3); 
  lightMeter.begin();

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected!");
}

void loop() {
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  int soilMoisture = analogRead(SOIL_PIN);
  float lux = lightMeter.readLightLevel();

  Serial.print("Temp: "); Serial.print(temperature);
  Serial.print(" C, Humidity: "); Serial.print(humidity);
  Serial.print(" %, Soil: "); Serial.print(soilMoisture);
  Serial.print(", Light: "); Serial.print(lux); Serial.println(" lx");

  if(WiFi.status() == WL_CONNECTED){
    HTTPClient http;
    String url = server + "?api_key=" + apiKey + 
                 "&field1=" + String(temperature) +
                 "&field2=" + String(humidity) +
                 "&field3=" + String(soilMoisture) +
                 "&field4=" + String(lux);
    http.begin(url);
    int httpResponseCode = http.GET();
    if(httpResponseCode>0){
      Serial.println("Data sent to ThingSpeak successfully");
    } else {
      Serial.println("Error sending data to ThingSpeak");
    }
    http.end();
  }

  
  if(soilMoisture < 500)
  {  
    digitalWrite(RELAY_PIN, HIGH);
    Serial.println("Pump ON");
    delay(5000); 
    digitalWrite(RELAY_PIN, LOW);
  }

  delay(20000); 
}
