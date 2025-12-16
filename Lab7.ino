#include <WiFi.h>
#include <HTTPClient.h>

// *** แก้ไข: กำหนดขา LED เป็น 15 (D15) ***
#define LED 15 

// เปลี่ยนเป็นข้อมูล Wi-Fi ของคุณ
const char* ssid = "IOT-RMUTI";
const char* password = "@1111111111111!";

// URL ของ Web App
const char* scriptUrl = "https://script.google.com/macros/s/AKfycbw0b7-CfCuE2qaJFoQFy3s4LY585MdXBbIv7zcS6uaATxjARjxFiTkBecSo0glJ_i2nEQ/exec";

void setup() {
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  
  // 1. เชื่อมต่อ Wi-Fi
  Serial.print("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected.");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // 2. เรียกใช้ฟังก์ชันเพื่ออ่านข้อมูลครั้งแรก
  readGoogleSheetValue();
}

void loop() {
  // เรียกอ่านข้อมูลซ้ำทุกๆ 5 วินาที
  delay(5000);
  readGoogleSheetValue();
}

void readGoogleSheetValue() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    Serial.println("-> Starting HTTP request...");
    http.begin(scriptUrl);
    
    // ส่งคำขอ HTTP GET
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
    int httpCode = http.GET();
    Serial.print("-> HTTP Response Code: ");
    Serial.println(httpCode);
    
    if (httpCode > 0) {
      if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        
        Serial.print("Value from Google Sheet (B2): ");
        Serial.println(payload);
        
        // แปลง payload เป็นตัวเลข
        int valueB2 = payload.toInt();
        
        // *** ตรวจสอบค่า: ถ้า B2 เป็น 1 ให้ไฟ LED (D15) ติด ***
        if (valueB2 == 1) {
          Serial.println("-> Action: Value is 1 (ON/TRUE)");
          digitalWrite(LED, HIGH); // ไฟติด
        } else if (valueB2 == 0) {
          Serial.println("-> Action: Value is 0 (OFF/FALSE)");
          digitalWrite(LED, LOW);  // ไฟดับ
        } else {
          Serial.println("-> Action: Other value detected.");
        }
        
      }
    } else {
      Serial.print("Error on HTTP request: ");
      Serial.println(httpCode);
    }
    http.end(); // ปิดการเชื่อมต่อ
  } else {
    Serial.println("WiFi Disconnected. Reconnecting...");
  }
}