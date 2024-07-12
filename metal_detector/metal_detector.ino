#include <ESP32Servo.h>
#include <WiFi.h>

const char* ssid = "your_SSID"; // Ini diganti nama wifi
const char* password = "your_PASSWORD"; // Ini diganti password wifi

const char* baseRoute = "http://10.210.123.38:5000"; // Ngambil dari cmd

bool isMetal = false;
int metalSensorPin = 14;

int servoPin = 14;
int servoIdleDegree = 0; // Derajat servo di mode diam
int servoMovedDegree = 90; // Derajat servo di mode bergerak

void setup() {
  Serial.begin(9600);
  pinMode(metalSensorPin, INPUT);

  servo.attach(servoPin); 

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  servo.write(servoIdleDegree);
}

void loop() {
  int metalSensorValue = digitalRead(metalSensorPin);

  if (!isMetal && metalSensorValue == 0) {
    isMetal = true;
    onMetalDetected();
  } else if (isMetal && metalSensorValue != 0){
    isMetal = false;
    onMetalUndetected();
  }
  delay(100);
}

void onMetalDetected(){
  Serial.println("Metal detected!");
  servo.write(servoMovedDegree);

  WiFiClient client;
  HTTPClient http;

  http.begin(client, baseRoute+"/metal");
  
  int code = http.POST();
  if (code >= 200) Serial.println("Sukses mengirim data ke api!");
  else Serial.println("Gagal mengirim data ke api!");
  http.end();
}

void onMetalUndetected(){
  Serial.println("Metal undetected!");
  servo.write(servoIdleDegree);
}
