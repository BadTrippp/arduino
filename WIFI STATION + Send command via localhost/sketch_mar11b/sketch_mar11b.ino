#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include "freertos/FreeRTOS.h"
 
#include <Adafruit_BME280.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_NeoPixel.h>


const char *SSID = "Nha Tro Lau 2";
const char *PWD = "p@ssword123!";


const char* ssid = "ESP32-Station"; // The SSID of your ESP32 module
const char* password = "esp32password"; // The password for your ESP32 module's Wi-Fi network

#define NUM_OF_LEDS 8 
#define PIN 4
 
// Web server running on port 80
WebServer server(80);
 
// Sensor
Adafruit_BME280 bme;
// Neopixel LEDs strip
Adafruit_NeoPixel pixels(NUM_OF_LEDS, PIN, NEO_GRB + NEO_KHZ800);
 
// JSON data buffer
StaticJsonDocument<250> jsonDocument;
char buffer[250];
 
// env variable
int temperature;
int humidity;
int pressure;

void wifiStation() {
  WiFi.softAP(ssid, password);
  Serial.print("ESP32 Station running, SSID: ");
  Serial.println(ssid);
  Serial.print("Password: ");
  Serial.println(password);

  Serial.print("IP address: ");
  Serial.println(WiFi.softAPIP());
}

void connectToWiFi() {
  Serial.print("Connecting to ");
  Serial.println(SSID);
  
  WiFi.begin(SSID, PWD);
  
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
    // we can even make the ESP32 to sleep
  }
 
  Serial.print("Connected. IP: ");
  Serial.println(WiFi.localIP());
}
void setup_routing() {	 	 
  server.on("/temperature", getTemperature);	 	 
  server.on("/led", HTTP_POST, handlePost);	 	 
  	 	 
  // start server	 	 
  server.begin();	 	 
}
 
void create_json(char *tag, int value, char *unit) {  
  jsonDocument.clear();  
  jsonDocument["type"] = tag;
  jsonDocument["value"] = value;
  jsonDocument["unit"] = unit;
  serializeJson(jsonDocument, buffer);
}
 
void add_json_object(char *tag, int value, char *unit) {
  JsonObject obj = jsonDocument.createNestedObject();
  obj["type"] = tag;
  obj["value"] = value;
  obj["unit"] = unit; 
}
void read_sensor_data(void * parameter) {
   for (;;) {
     temperature = bme.readTemperature();
     humidity = bme.readHumidity();
     pressure = bme.readPressure() / 100;
     Serial.println("Read sensor data");
 
     // delay the task
     vTaskDelay(60000 / portTICK_PERIOD_MS);
   }
}
 
void getTemperature() {
  Serial.println("Signal");
  create_json("Singal: ", 1 , "UP");
  server.send(200, "application/json", buffer);
}
void handlePost() {
  if (server.hasArg("plain") == false) {
    //handle error here
  }
  String body = server.arg("plain");
  deserializeJson(jsonDocument, body);
  
  // Respond to the client
  server.send(200, "application/json", "{}");
}
void setup_task() {	 	 
  xTaskCreate(	 	 
  read_sensor_data, 	 	 
  "Read sensor data", 	 	 
  1000, 	 	 
  NULL, 	 	 
  1, 	 	 
  NULL 	 	 
  );	 	 
}
void setup() {	 	 
  Serial.begin(9600);	 	 
 	 	 
  // Sensor setup	 
  wifiStation();	 
  // connectToWiFi();	 	  	 
  setup_routing(); 	 	 
  // Initialize Neopixel	 	 
}	 	 
  	 	 
void loop() {	 	 
  server.handleClient();	 	 
}