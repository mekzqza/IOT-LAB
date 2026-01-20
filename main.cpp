#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <RTClib.h>

// --- Config ---
const char* ssid = "Wokwi-GUEST";
const char* password = "";
String serverName = "https://roblox-api.sukpat.dev/api/collections/smart_farm/records";

// Pins
const int vrTempPin = 35; 
const int vrHumPin = 34;
const int vrLighPin = 33; 
const int vrSoilPin = 32;

const int ledBluePin = 4;
const int ledGreenPin = 2;
const int ledRedPin = 15;

unsigned long lastSendTime = 0;
String lastSentTimeDisplay = "Wait..."; // ตัวแปรเก็บเวลาส่งล่าสุด 

LiquidCrystal_I2C lcd(0x27, 20, 4);
RTC_DS1307 rtc; 

void setup() {
  Serial.begin(115200);
  pinMode(ledBluePin, OUTPUT); pinMode(ledGreenPin, OUTPUT); pinMode(ledRedPin, OUTPUT);
  
  lcd.init(); lcd.backlight();
  if (!rtc.begin()) Serial.println("RTC Fail");
  if (!rtc.isrunning()) rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  lcd.setCursor(0, 0); 
  lcd.print("Connecting WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); Serial.print(".");
  }
  lcd.clear(); 
}

void loop() {
  // 1. อัปเดตนาฬิกา (บรรทัดบน)
  DateTime now = rtc.now();
  String sep = (now.second() % 2 == 0) ? ":" : " ";
  char timeStr[20];
  sprintf(timeStr, "%02d/%02d %02d%s%02d%s%02d", 
          now.day(), now.month(), now.hour(), sep.c_str(), now.minute(), sep.c_str(), now.second());
  
  lcd.setCursor(0, 0); lcd.print(timeStr);

  // 2. อ่านค่า Sensor
  float temp = map(analogRead(vrTempPin), 0, 4095, 0, 400) / 10.0;
  int hum = map(analogRead(vrHumPin), 0, 4095, 20, 90);
  int lux = map(analogRead(vrLighPin), 0, 4095, 0, 10000);
  int rawSoil = analogRead(vrSoilPin);
  int soilShow = map(rawSoil, 0, 4095, 0, 100); 
  float soilVoltage = rawSoil * (5.0 / 4095.0); 

  // LED Logic
  String currentStatus = "";
  if (soilVoltage <= 2.7) {
    currentStatus = "WET"; digitalWrite(ledBluePin, HIGH); digitalWrite(ledGreenPin, LOW); digitalWrite(ledRedPin, LOW);
  } else if (soilVoltage <= 3.1) {
    currentStatus = "MOIST"; digitalWrite(ledBluePin, LOW); digitalWrite(ledGreenPin, HIGH); digitalWrite(ledRedPin, LOW);
  } else {
    currentStatus = "DRY"; digitalWrite(ledBluePin, LOW); digitalWrite(ledGreenPin, LOW); digitalWrite(ledRedPin, HIGH);
  }

  // 3. แสดงผลหน้าจอ (Line 1-2)
  lcd.setCursor(0, 1); 
  lcd.print("T:" + String(temp, 0) + " H:" + String(hum) + "% L:" + String(lux) + "  ");
  
  lcd.setCursor(0, 2); 
  lcd.print("S:" + String(soilShow) + "% St:" + currentStatus + "     ");

  // 4. แสดงเวลาที่ส่งล่าสุด (Line 3) - ค่านี้จะค้างไว้จนกว่าจะส่งรอบใหม่ผ่าน
  lcd.setCursor(0, 3); 
  lcd.print("Sent: " + lastSentTimeDisplay);

  // ==========================================
  // ส่วนส่งข้อมูล (ทำงานทุก 5 วินาที)
  // ==========================================
  if (millis() - lastSendTime > 5000) {
    if(WiFi.status() == WL_CONNECTED) {
      lcd.setCursor(19, 0); lcd.print("*"); // ขึ้นดาวบอกว่ากำลังส่ง (ช่วงนี้จอจะค้างแป๊บนึง)

      HTTPClient http;
      http.setTimeout(3000); 
      http.begin(serverName); 
      http.addHeader("Content-Type", "application/json");
      
      // >>> สร้าง String เวลาสำหรับส่ง Server (Format: YYYY-MM-DD HH:MM:SS) <<<
      char deviceTimeBuffer[25];
      sprintf(deviceTimeBuffer, "%04d-%02d-%02d %02d:%02d:%02d", 
              now.year(), now.month(), now.day(), 
              now.hour(), now.minute(), now.second());

      // >>> เพิ่ม device_time เข้าไปใน JSON <<<
      String jsonPayload = "{\"temperature\":" + String(temp) + 
                           ", \"humidity\":" + String(hum) + 
                           ", \"light\":" + String(lux) + 
                           ", \"soil\":" + String(soilShow) + 
                           ", \"soil_status\":\"" + currentStatus + "\"" +
                           ", \"device_time\":\"" + String(deviceTimeBuffer) + "\"}";

      int httpCode = http.POST(jsonPayload);
      
      // ถ้าส่งผ่าน (Code > 0) ให้เซฟเวลาตอนนี้เก็บไว้โชว์
      if (httpCode > 0) {
        char sentTimeBuffer[10];
        sprintf(sentTimeBuffer, "%02d:%02d:%02d", now.hour(), now.minute(), now.second());
        lastSentTimeDisplay = String(sentTimeBuffer) + " OK"; 
      } else {
        lastSentTimeDisplay = "Error " + String(httpCode);
      }
      
      http.end();
      lcd.setCursor(19, 0); lcd.print(" "); // ลบดาว
    }
    lastSendTime = millis();
  }
}