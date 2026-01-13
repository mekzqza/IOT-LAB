#include <WiFi.h>
#include <HTTPClient.h>

// 1. ตั้งค่า WiFi (แก้ตรงนี้ให้เป็นของคุณ)
const char* ssid = "your wifi";       // <-- แก้ชื่อ WiFi
const char* password = "your password"; // <-- แก้รหัสผ่าน WiFi

// 2. ตั้งค่า Server (แก้ IP ให้ตรงกับเครื่องคุณ)
String serverName = "http://172.24.149.42/iot_project/insert_data.php"; 

// กำหนดขา Pin ของ VR (ตามที่คุณบอกมา)
const int vrTempPin = 35; // VR ตัวที่ 1 ต่อขา D35 (Temperature)
const int vrHumPin = 34;  // VR ตัวที่ 2 ต่อขา D34 (Humidity)

String sensorName = "ESP32_VR_Lab"; // เปลี่ยนชื่อ Sensor หน่อยจะได้รู้ว่ามาจากแล็บ VR
float temperature = 0;
float humidity = 0;

unsigned long lastTime = 0;
unsigned long timerDelay = 5000; // ส่งทุก 5 วินาที

void setup() {
  Serial.begin(115200);

  // เชื่อมต่อ WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // ตรวจสอบว่าถึงเวลาส่งข้อมูลหรือยัง
  if ((millis() - lastTime) > timerDelay) {
    
    if(WiFi.status() == WL_CONNECTED) {
      
      // --- ส่วนที่แก้ไข: อ่านค่าจาก VR D35 และ D34 ---
      
      // 1. อ่านค่าจากขา D35 (Temperature)
      int rawTemp = analogRead(vrTempPin); 
      // แปลงค่าจาก 0-4095 เป็น 0-50 องศาเซลเซียส (สมมติ)
      temperature = map(rawTemp, 0, 4095, 0, 50); 

      // 2. อ่านค่าจากขา D34 (Humidity)
      int rawHum = analogRead(vrHumPin);
      // แปลงค่าจาก 0-4095 เป็น 0-100 เปอร์เซ็นต์
      humidity = map(rawHum, 0, 4095, 0, 100);

      // แสดงค่าที่อ่านได้ดูใน Serial Monitor ก่อนส่ง
      Serial.print("Read VR D35 (Temp): "); Serial.print(temperature);
      Serial.print(" | Read VR D34 (Hum): "); Serial.println(humidity);

      // ---------------------------------------------

      HTTPClient http;
      http.begin(serverName);
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
      
      String httpRequestData = "sensor_name=" + sensorName 
                             + "&temperature=" + String(temperature)
                             + "&humidity=" + String(humidity);
      
      Serial.print("Sending data: ");
      Serial.println(httpRequestData);

      int httpResponseCode = http.POST(httpRequestData);

      if (httpResponseCode > 0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        String payload = http.getString();
        Serial.println("Server says: " + payload);
      }
      else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
      }
      http.end();
    }
    else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }
}