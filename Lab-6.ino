#include <WiFi.h>
#include <HTTPClient.h>
#define LED 23
// เปลี่ยนเป็นข้อมูล Wi-Fi ของคุณ
const char* ssid = "IOT-RMUTI";
const char* password = "@1111111111111!";


// เปลี่ยนเป็น URL ของ Web App ที่คุณได้จากขั้นตอนที่ 1
const char* scriptUrl = "https://script.google.com/macros/s/AKfycbytv9BI8sBorlOnpPVm1dd3liOhV--RnWYr4StZ9S8qgDCOW_3uJa1LL0usLof1IuWmnQ/exec";


void setup() {
  Serial.begin(115200);
  pinMode(LED,OUTPUT);
 
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
  // สามารถตั้งเวลาให้เรียกอ่านข้อมูลซ้ำได้ทุกๆ 5 วินาที
  delay(5000);
  readGoogleSheetValue();
}


void readGoogleSheetValue() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    Serial.println("-> Starting HTTP request..."); // เพิ่มบรรทัดนี้
    http.begin(scriptUrl);
   
   
    // ส่งคำขอ HTTP GET
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS); //****
    int httpCode = http.GET();
    Serial.print("-> HTTP Response Code: "); // เพิ่มบรรทัดนี้
    Serial.println(httpCode);              // เพิ่มบรรทัดนี้
   
    if (httpCode > 0) {
      // HTTP code 200 คือสำเร็จ
      if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
       
        // ข้อมูลที่ได้คือค่าในช่อง A1 (1 หรือ 0)
        Serial.print("Value from Google Sheet (A1): ");
        Serial.println(payload);
       
        // **การนำค่าไปใช้งาน:**
        // แปลง payload เป็นตัวเลขเพื่อนำไปใช้ควบคุมอื่นๆ
        int valueA1 = payload.toInt();
       
        if (valueA1 == 1) {
          Serial.println("-> Action: Value is 1 (ON/TRUE)");
          digitalWrite(LED,HIGH);
          // ใส่โค้ดควบคุมอุปกรณ์เมื่อ A1 เป็น 1
        } else if (valueA1 == 0) {
          Serial.println("-> Action: Value is 0 (OFF/FALSE)");
          digitalWrite(LED,LOW);
          // ใส่โค้ดควบคุมอุปกรณ์เมื่อ A1 เป็น 0
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
    // โค้ดสำหรับพยายามเชื่อมต่อใหม่สามารถเพิ่มที่นี่ได้
  }
}

