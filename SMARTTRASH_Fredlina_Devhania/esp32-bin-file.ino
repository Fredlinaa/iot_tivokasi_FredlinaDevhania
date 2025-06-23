#include <WiFi.h>
#include <PubSubClient.h>
#include <ESP32Servo.h>

// Pin Konfigurasi
#define TRIG_PIN 5
#define ECHO_PIN 18
#define LED_PIN 22
#define SERVO_PIN 21
#define IR_PIN 19

// WiFi dan MQTT
const char* ssid = "Wokwi-GUEST";
const char* password = "";
const char* mqtt_server = "broker.hivemq.com";

WiFiClient espClient;
PubSubClient client(espClient);
Servo servo;
bool isOpen = false;

void setup_wifi() {
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi!");
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("esp32TrashClient")) {
      Serial.println("Connected to MQTT");
    } else {
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);

  // Setup pin
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(IR_PIN, INPUT);

  // Setup servo
  servo.setPeriodHertz(50);
  servo.attach(SERVO_PIN, 500, 2400);
  servo.write(0); // Tutup awal
  isOpen = false;

  // Setup WiFi dan MQTT
  setup_wifi();
  client.setServer(mqtt_server, 1883);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Baca sensor IR
  int irValue = digitalRead(IR_PIN);
  bool objectDetected = irValue == 0; // 0 artinya benda terdeteksi

  // Baca jarak dengan sensor ultrasonik
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long duration = pulseIn(ECHO_PIN, HIGH);
  float distance = duration * 0.034 / 2;

  // Kontrol Servo
  if (distance < 20 && !isOpen) {
    servo.write(90);  // Buka
    isOpen = true;
    delay(3000);
  } else if (distance >= 20 && isOpen) {
    servo.write(0);   // Tutup
    isOpen = false;
  }

  // Kontrol LED dari IR sensor
  digitalWrite(LED_PIN, objectDetected ? HIGH : LOW);

  // Publish data ke MQTT
  client.publish("iot/trash/distance", String(distance).c_str());
  client.publish("iot/trash/led", objectDetected ? "ON" : "OFF");
  client.publish("iot/trash/servo", distance < 15 ? "AKTIF" : "NONAKTIF");
  client.publish("iot/trash/status", objectDetected ? "PENUH" : "AMAN");

  // Debug ke serial monitor
  Serial.print("Jarak: ");
  Serial.print(distance);
  Serial.print(" cm | IR: ");
  Serial.print(irValue);
  Serial.print(" | Servo: ");
  Serial.print(isOpen ? "Buka" : "Tutup");
  Serial.print(" | LED: ");
  Serial.print(objectDetected ? "ON" : "OFF");
  Serial.print(" | Status: ");
  Serial.println(objectDetected ? "PENUH" : "AMAN");

  delay(1000); // Delay antar pembacaan
}
