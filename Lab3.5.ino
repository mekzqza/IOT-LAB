/**************************************************************
 *  ESP32 + WiFiManager + Blynk IoT (Cloud)
 *  ใช้งานกับ Template Quickstart (ของอาจารย์)
 **************************************************************/

#define BLYNK_TEMPLATE_ID "TMPL6vtu2Hpmv"
#define BLYNK_TEMPLATE_NAME "Quickstart Template"
#define BLYNK_AUTH_TOKEN "IPl6FqNpKJAF3lw4wRPhhnEM6NroOKre"

#include <WiFi.h>
#include <WiFiManager.h>
#include <BlynkSimpleEsp32.h>

#define RESET_PIN 13     // ปุ่มรีเซ็ต WiFi (กดติดกับ GND)
#define LED_PIN 2        // LED ที่บอร์ด (GPIO 2)

WiFiManager wm;

// ฟังก์ชันรับค่าจาก Blynk → V12 (ปุ่ม)
BLYNK_WRITE(V12)
{
  int ledstatus = param.asInt();
  Serial.print("Blynk Button Value: ");
  Serial.println(ledstatus);

  // ควบคุม LED จริงบน ESP32
  digitalWrite(LED_PIN, ledstatus);

  // ส่งค่าไปโชว์ที่ Widget LED (V0)
  Blynk.virtualWrite(V0, ledstatus);
}

// ตัวแปรกันกดค้าง
unsigned long pressStartTime = 0;
const unsigned long holdTime = 3000;  // 3 วิ

void setup() {
  Serial.begin(115200);
  delay(500);

  pinMode(RESET_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);

  Serial.println("\n--- ESP32 WiFiManager + Blynk (Quickstart) ---");

  // ตรวจสอบการกดปุ่ม RESET WiFi
  if (digitalRead(RESET_PIN) == LOW) {
    Serial.println("Press to reset WiFi...");
    pressStartTime = millis();

    while (digitalRead(RESET_PIN) == LOW) {
      if (millis() - pressStartTime >= holdTime) {
        Serial.println("WiFi Reset Confirmed!");
        wm.resetSettings();
        delay(1000);
        ESP.restart();
      }
      delay(10);
    }
  }

  // Timeout 30 วินาที ถ้าเชื่อมไม่ได้ → เปิด AP ใหม่
  wm.setConnectTimeout(30);

  // เริ่ม WiFiManager
  bool res = wm.autoConnect("ESP32-SetupAP", "12345678");

  if (!res) {
    Serial.println("❌ WiFi failed → Restarting...");
    ESP.restart();
  } else {
    Serial.println("✅ WiFi Connected");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
  }

  // เชื่อมต่อ Blynk Cloud
  Blynk.config(BLYNK_AUTH_TOKEN);
  if (!Blynk.connect()) {
    Serial.println("❌ Blynk Connect Failed");
  } else {
    Serial.println("✅ Connected to Blynk!");
  }
}

void loop() {
  Blynk.run();

  // ส่งค่า Random → V10 ทุก 5 วินาที
  static unsigned long lastSend = 0;
  if (millis() - lastSend > 5000) {
    int sensorValue = random(20, 30);
    Serial.printf("Send Random Data → V10 = %d\n", sensorValue);

    Blynk.virtualWrite(V10, sensorValue);
    lastSend = millis();
  }
}
