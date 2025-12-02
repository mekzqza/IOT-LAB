#include <WiFi.h>
#include <WiFiManager.h>
#include <HTTPClient.h>

#define RESET_PIN 0   
String apiKey = "E3FFINV81ORXNW8P";   // ⭐ ใส่ Write API Key ของคุณ
const char* server = "http://api.thingspeak.com/update";

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\nStarting...");

  pinMode(RESET_PIN, INPUT_PULLUP);

  WiFiManager wm;

  if (digitalRead(RESET_PIN) == LOW) {
    Serial.println("WiFi Reset Button Pressed!");
    wm.resetSettings();
    delay(1000);
    ESP.restart();
  }

  bool res = wm.autoConnect("ESP32-mekzqza007", "12345678");

  if (!res) {
    Serial.println("Failed to connect. Restarting...");
    ESP.restart();
  } else {
    Serial.println("WiFi Connected!");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
  }
}

void loop() {

  if (WiFi.status() == WL_CONNECTED) {

    HTTPClient http;

    // ⭐ สุ่มค่า Sensor
    float temperature = random(250, 350) / 10.0;   // 25.0 - 35.0
    float humidity    = random(500, 800) / 10.0;   // 50.0 - 80.0

    // ⭐ เตรียม URL ส่งขึ้น ThingSpeak
    String url = String(server) +
                 "?api_key=" + apiKey +
                 "&field1=" + String(temperature) +
                 "&field2=" + String(humidity);

    Serial.println("ส่ง URL:");
    Serial.println(url);

    http.begin(url);
    int httpResponseCode = http.GET();

    if (httpResponseCode > 0) {
      Serial.println("✔ ส่งข้อมูลขึ้น ThingSpeak สำเร็จ");
    } else {
      Serial.println("❌ ส่งข้อมูลไม่สำเร็จ");
    }

    http.end();

  } else {
    Serial.println("WiFi not connected!");
  }

  delay(20000);   // ต้อง >= 15 วิ ตามกฎของ ThingSpeak
}
