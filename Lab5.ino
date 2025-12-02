#include <WiFi.h>
#include <HTTPClient.h>

// === ใส่ WiFi ของคุณ ===
const char* ssid     = "IOT-RMUTI";
const char* password = "@1111111111111!";

// === ใส่ URL ของ Google Apps Script Web App ===
String url = "Scripts https://script.google.com/macros/s/AKfycbz85a-L7FJaT1hWzWPWf-rHXsqBjdglcF-2a4xH_UsIQpQfmsYXAoZvQuG51eo_IdNwZw/exec";  // เช่น https://script.google.com/macros/s/xxxx/exec

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected");
}

void loop() {
  // Random ค่า Temperature และ Humidity
  float temp = random(200, 350) / 10.0;   // 20.0 - 35.0 °C
  float humi = random(300, 900) / 10.0;   // 30.0 - 90.0 %

  // สร้าง JSON
  String jsonData = "{";
  jsonData += "\"temp\":" + String(temp) + ",";
  jsonData += "\"humi\":" + String(humi);
  jsonData += "}";

  Serial.println("Sending JSON:");
  Serial.println(jsonData);

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    http.begin(url);
    http.addHeader("Content-Type", "application/json");

    int httpResponseCode = http.POST(jsonData);

    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);

    String payload = http.getString();
    Serial.println("Response: " + payload);

    http.end();
  }

  delay(10000); // ส่งทุก 10 วินาที
}


