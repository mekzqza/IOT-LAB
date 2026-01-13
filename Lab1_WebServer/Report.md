# รายงานผลการทดลอง Lab 1: การเชื่อมต่อ ESP32 กับ Web Server และฐานข้อมูล MySQL

**ชื่อผู้จัดทำ:** [ใส่ชื่อ-นามสกุลของคุณ]
**รหัสนักศึกษา:** [ใส่รหัสนักศึกษา]
**วิชา:** Internet of Things (IoT)

---

## 1. บทนำและวัตถุประสงค์
[cite_start]โปรเจกต์นี้มีวัตถุประสงค์เพื่อสร้างระบบ IoT แบบ End-to-End โดยใช้อุปกรณ์ **ESP32** อ่านค่าจากเซนเซอร์จำลอง (ตัวต้านทานปรับค่าได้ - VR) ส่งข้อมูลผ่าน WiFi ไปยัง **Web Server (XAMPP)** เพื่อบันทึกลงฐานข้อมูล **MySQL** และนำข้อมูลมาแสดงผลเป็นกราฟบนหน้าเว็บแบบ Real-time [cite: 2, 4-7]

---

## 2. เครื่องมือและอุปกรณ์ที่ใช้
1.  **Hardware:**
    * บอร์ด ESP32
    * ตัวต้านทานปรับค่าได้ (VR) จำนวน 2 ตัว (จำลอง Temperature และ Humidity)
    * สายจั๊มเปอร์และ Breadboard
2.  **Software:**
    * Arduino IDE (สำหรับเขียนโปรแกรมลง ESP32)
    * XAMPP (จำลอง Web Server, Database, PHP)
    * Visual Studio Code หรือ Notepad (สำหรับเขียน Web/PHP)
    * Web Browser (Chrome/Edge)

---

## 3. ขั้นตอนการดำเนินงาน (Implementation Steps)

### ส่วนที่ 1: การเตรียมฝั่ง Server (Database & PHP)
1.  **ติดตั้งและตั้งค่า XAMPP:**
    * เปิดบริการ **Apache** (Web Server) และ **MySQL** (Database)
    * แก้ปัญหา Port ชน โดยการปิด Service MySQL เดิมใน Windows Services
    * [cite_start]**การตั้งค่าความปลอดภัย:** แก้ไขไฟล์ `httpd-xampp.conf` โดยเปลี่ยนจาก `Require local` เป็น `Require all granted` เพื่อให้ ESP32 จากภายนอกสามารถยิงข้อมูลเข้ามาได้ [cite: 564-566]

2.  **สร้างฐานข้อมูล (Database Setup):**
    * เข้าใช้งานผ่าน `http://localhost/phpmyadmin`
    * สร้าง Database ชื่อ `iot_project`
    * สร้าง Table ชื่อ `sensor_data` ประกอบด้วยคอลัมน์:
        * `id` (INT, Auto Increment, PK)
        * `sensor_name` (VARCHAR)
        * `temperature` (FLOAT)
        * `humidity` (FLOAT)
        * [cite_start]`created_at` (TIMESTAMP) [cite: 535-541]

3.  **เขียนสคริปต์ฝั่ง Server (Backend):**
    * สร้างโฟลเดอร์ `C:\xampp\htdocs\iot_project`
    * [cite_start]**`insert_data.php`:** สำหรับรับค่า POST Request จาก ESP32 และบันทึกลง MySQL [cite: 546, 574]
    * [cite_start]**`fetch_data.php`:** สำหรับดึงข้อมูลล่าสุด 20 รายการจาก MySQL และส่งออกเป็น JSON สำหรับทำกราฟ [cite: 547, 613]

### ส่วนที่ 2: การเตรียมฝั่ง Hardware (ESP32)
1.  **การต่อวงจร:**
    * VR ตัวที่ 1 (Temperature) ต่อขา **D35**
    * VR ตัวที่ 2 (Humidity) ต่อขา **D34**
    * (หมายเหตุ: ใช้ขา Input-only ในการอ่านค่า Analog)

2.  **เขียนโปรแกรม Arduino:**
    * ใช้ Library `WiFi.h` และ `HTTPClient.h`
    * **การทำงาน:**
        1.  เชื่อมต่อ WiFi (SSID/Password)
        2.  อ่านค่า Analog จากขา D35 และ D34
        3.  ใช้ฟังก์ชัน `map()` แปลงค่าดิบ (0-4095) เป็นหน่วยองศาเซลเซียสและเปอร์เซ็นต์
        4.  แก้ปัญหา **Signal Crosstalk** (ค่ากวนกัน) โดยใช้เทคนิคอ่านค่าทิ้ง 1 รอบ (`analogRead` 2 ครั้ง)
        5.  [cite_start]ส่งข้อมูลผ่าน HTTP POST ไปยัง Server ตาม IP Address ของเครื่องคอมพิวเตอร์ (เช่น `172.xx.xx.xx`) [cite: 665-718]

### ส่วนที่ 3: การแสดงผล (Dashboard)
1.  สร้างไฟล์ **`index.html`** ในโฟลเดอร์โปรเจกต์
2.  ใช้ Library **Chart.js** เพื่อสร้างกราฟเส้น (Line Chart)
3.  [cite_start]เขียน JavaScript ให้ดึงข้อมูลจาก `fetch_data.php` ทุกๆ 2 วินาที (Interval) เพื่ออัปเดตกราฟให้เป็น Real-time [cite: 850-901]

---

## 4. ผลการทดลอง
1.  **การเชื่อมต่อ:** ESP32 สามารถเชื่อมต่อ WiFi และส่งข้อมูลเข้า Server ได้สำเร็จ (ตรวจสอบผ่าน Serial Monitor ขึ้นสถานะ `Save OK!`)
2.  **ฐานข้อมูล:** ข้อมูล `temperature` และ `humidity` ถูกบันทึกลงตาราง `sensor_data` อย่างถูกต้อง
3.  **หน้าเว็บ:** สามารถแสดงกราฟเส้น 2 เส้น (แดง/น้ำเงิน) ที่เปลี่ยนแปลงตามการหมุน VR ได้อย่างถูกต้องและต่อเนื่อง

---

## 5. ปัญหาที่พบและการแก้ไข
* **ปัญหา:** เข้า phpMyAdmin ไม่ได้เนื่องจาก Port 3306 ชนกับ MariaDB ตัวเก่า
    * **แก้ไข:** ทำการ Stop Service MariaDB ใน Windows Services แล้ว Start MySQL ของ XAMPP ใหม่
* **ปัญหา:** ESP32 ส่งข้อมูลไม่เข้า (Error 403 Forbidden)
    * **แก้ไข:** ปรับ Config ของ Apache ให้ `Require all granted` เพื่ออนุญาต IP ภายนอก
* **ปัญหา:** ค่า Sensor กวนกัน (หมุนตัวหนึ่ง อีกตัวเปลี่ยนตาม)
    * **แก้ไข:** เพิ่ม Code ให้ `analogRead()` ทิ้ง 1 ครั้งก่อนอ่านจริง และตรวจสอบจุดต่อสายให้แน่นหนา

---

## 6. สรุปผล
การทดลองนี้ทำให้เข้าใจกระบวนการทำงานของระบบ IoT ตั้งแต่ต้นน้ำ (Sensor/ESP32) ไปจนถึงปลายน้ำ (Server/Dashboard) โดยสามารถประยุกต์ใช้ **REST API** ในการรับส่งข้อมูล และใช้ **MySQL** ในการจัดเก็บข้อมูลได้อย่างมีประสิทธิภาพ ตามวัตถุประสงค์ของรายวิชา