#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
#define DHTPIN 2
#define DHTTYPE DHT11
#define LDR_AO 34  // Pin untuk Analog Output dari LDR
#define LDR_DO 27  // Pin untuk Digital Output dari LDR

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
DHT dht(DHTPIN, DHTTYPE);

void setup() {
    Serial.begin(115200);
    dht.begin();
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    pinMode(LDR_DO, INPUT);
}

void loop() {
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();
    int lightIntensity = analogRead(LDR_AO);
    int lightStatus = digitalRead(LDR_DO);

    display.clearDisplay();
    display.setCursor(0, 0);
    display.printf("Suhu: %.1f C\n", temperature);
    display.printf("Kelembapan: %.1f%%\n", humidity);
    display.printf("Cahaya: %d lux\n", lightIntensity);
    display.printf("Status: %s\n", lightStatus ? "Terang" : "Gelap");
    display.display();
    
    Serial.print("Suhu: ");
    Serial.print(temperature);
    Serial.println(" C");
    Serial.print("Kelembapan: ");
    Serial.print(humidity);
    Serial.println(" %");
    Serial.print("Cahaya (Analog): ");
    Serial.println(lightIntensity);
    Serial.print("Cahaya (Digital): ");
    Serial.println(lightStatus ? "Terang" : "Gelap");
    
    delay(1000);
}
