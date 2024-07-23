
#include <WiFi.h>
#include <PubSubClient.h>
#include <ESP32Servo.h>

const char* ssid = "Android_87"; // Nama wifi
const char* password = "brainissue";
const char* mqtt_server = "192.168.1.23";

WiFiClient espClient;
PubSubClient client(espClient);

#define SERVO_PIN 32
#define METAL_PIN 14

#define TRIG_PIN1 26 // PIN1 untuk sensor yang deteksi sampah masuk,
#define ECHO_PIN1 27 // Kalau nggak akurat deteksi masuknya, bisa di coba pakai ir sensor
#define TRIG_PIN2 26 // PIN2 untuk di tempat sampah non logam
#define ECHO_PIN2 27
#define TRIG_PIN3 26 // PIN3 untuk di tempat sampah logam
#define ECHO_PIN3 27

#define METAL_CAPACITY 0 // Di tes dulu pakai yang distance meter
#define NON_METAL_CAPACITY 0 // Ini jg sama

#define SOUND_SPEED 0.034
#define CM_TO_INCH 0.393701

float ultrasonicThreshold = 15;
bool detected = false;

Servo servo;
int servoMetalDegree = 0;
int servoNonMetalDegree = 90;

int trashIn = 0;
long firstTrashInTime;

long mqttLastSend;

void setup() {
  Serial.begin(115200);
  pinMode(METAL_PIN, INPUT);
  pinMode(TRIG_PIN1, OUTPUT);
  pinMode(ECHO_PIN1, INPUT);
  pinMode(TRIG_PIN2, OUTPUT);
  pinMode(ECHO_PIN2, INPUT);
  pinMode(TRIG_PIN3, OUTPUT);
  pinMode(ECHO_PIN3, INPUT);

  setupWiFi();
  client.setServer(mqtt_server, 1883);
}

void setupWiFi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" connected");
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ArduinoClient")) {
      Serial.println("connected");
      client.subscribe("sensor");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

float readUltrasonic(int trigPin, int echoPin){
  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Calculate the distance
  return pulseIn(echoPin, HIGH) * SOUND_SPEED/2;
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  bool isMetal = digitalRead(METAL_PIN) != HIGH;
  if (isMetal) {
    servo.write(servoMetalDegree);
  }else{
    servo.write(servoNonMetalDegree);
  } 

  if ((millis()-firstTrashInTime) >= 3600000) trashIn = 0;

  float distanceCm = readUltrasonic(TRIG_PIN1, ECHO_PIN1);
  if (distanceCm <= ultrasonicThreshold && !detected){
    Serial.println("Detect");
    detected = true;
    if (trashIn == 0) firstTrashInTime = millis();
    trashIn++;
    servo.write(servoNonMetalDegree);
  }else if (distanceCm > ultrasonicThreshold && detected) detected = false;
  
  if (millis() - mqttLastSend >= 1000){
    float heightNonLogam = readUltrasonic(TRIG_PIN2, ECHO_PIN2);
    float heightLogam = readUltrasonic(TRIG_PIN3, ECHO_PIN3);
  
    String jsonString = "{";
    jsonString += "\"sampah_per_jam\": " + String(trashIn) + ",";
    jsonString += "\"ketinggian_sampah_non_logam\": " + String(heightNonLogam) + ",";
    jsonString += "\"ketinggian_sampah_logam\": " + String(heightLogam) + ",";
    jsonString += "\"kapasitas_penuh_logam\": "+String(METAL_CAPACITY)+",";
    jsonString += "\"kapasitas_penuh_non_logam\": "+String(NON_METAL_CAPACITY);
    jsonString += "}";

    client.publish("sensor", jsonString.c_str());

    mqttLastSend = millis();
  }
  delay(100);
}
