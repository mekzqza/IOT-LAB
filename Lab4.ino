

#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>

// --- ตั้งค่า WiFi ---
const char* ssid = "IOT-RMUTI";
const char* password = "@1111111111111!";

// URL ของ Google Apps Script
const char* googleScriptURL = "https://script.google.com/macros/s/AKfycbwK35jfIaW1g9fUN4tDz75Lm4WHMxsm9gzCue-_ldHE7SiNueFOH4kTi5gNEYcl6--ndQ/exec";

void setup() {
  Serial.begin(115200);
  delay(1000);

  // เริ่มต้น WiFi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to Wi-Fi");
  
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // --- ส่วนจำลองข้อมูล (Simulation) ---
  // สุ่มค่าอุณหภูมิระหว่าง 20.0 ถึง 40.0 องศา
  float temperature = random(200, 400) / 10.0; 
  
  // สุ่มค่าความชื้นระหว่าง 40.0 ถึง 90.0 %
  float humidity = random(400, 900) / 10.0;

  Serial.print("Simulated Temp: ");
  Serial.print(temperature);
  Serial.print(" °C, Humi: ");
  Serial.print(humidity);
  Serial.println(" %");

  // --- ส่วนส่งข้อมูล ---
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClientSecure client;
    client.setInsecure(); // สำคัญ: ข้ามการตรวจสอบ SSL Certificate
    
    HTTPClient http;
    
    // สร้าง URL ส่งค่า
    String url = String(googleScriptURL) + "?temp=" + String(temperature) + "&humi=" + String(humidity);

    Serial.println("Sending data to Google Sheets...");

    if (http.begin(client, url)) { 
      // กำหนดให้ Redirect ตาม (ถ้า Google Script สั่งย้าย)
      http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);

      int httpCode = http.GET();
      
      if (httpCode > 0) {
        Serial.printf("[HTTP] GET code: %d\n", httpCode);
        if (httpCode == HTTP_CODE_OK || httpCode == 302) {
          Serial.println("Data sent successfully!");
        }
      } else {
        Serial.printf("[HTTP] GET failed, error: %s\n", http.errorToString(httpCode).c_str());
      }
      http.end();
    } else {
      Serial.println("[HTTP] Unable to connect");
    }
  } else {
    Serial.println("WiFi Disconnected");
  }

  delay(5000); // ส่งข้อมูลทุก 30 วินาที
}