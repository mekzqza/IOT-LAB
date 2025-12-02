#include <WiFi.h>
#include <WiFiManager.h>

#define RESET_PIN 12        // ปุ่ม Reset WiFi
#define GPIO_LED  4         // พินที่ต่อ LED แยก

WiFiManager wm;


void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("Starting ESP32 with external LED...");

  pinMode(GPIO_LED, OUTPUT);
  pinMode(RESET_PIN, INPUT_PULLUP);

  // ตรวจสอบปุ่ม Reset WiFi
  if (digitalRead(RESET_PIN) == LOW) {
    Serial.println("Button pressed... waiting 3 seconds");
    delay(3000);
    if (digitalRead(RESET_PIN) == LOW) {
      Serial.println("Reset WiFi settings!");
      wm.resetSettings();
      delay(500);
      ESP.restart();
    }
  }

  // เริ่มเชื่อมต่อ WiFi ด้วย WiFiManager
  bool res = wm.autoConnect("ESP32-mekzqza", "12345678");

  if (!res) {
    Serial.println("WiFi connect FAILED");
  } else {
    Serial.println("WiFi CONNECTED!");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
  }
  
}
void loop() {
 while(WiFi.status() != WL_CONNECTED){
        digitalWrite(GPIO_LED, HIGH);
        delay(300);
        Serial.println("เชื่อมต่อไม่ได้");
        digitalWrite(GPIO_LED, LOW);
        delay(300);
  }
  digitalWrite(GPIO_LED, HIGH); 
}

