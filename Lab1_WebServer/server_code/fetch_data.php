<?php
// ตั้งค่าการเชื่อมต่อ (เหมือนไฟล์ insert)
$servername = "localhost";
$username = "root";
$password = "";
$dbname = "iot_project";

// เชื่อมต่อ MySQL
$conn = new mysqli($servername, $username, $password, $dbname);

// เช็ค Error
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
}

// คำสั่งดึงข้อมูล: เอามาแค่ 20 ตัวล่าสุด เรียงจากเวลาล่าสุดไปเก่าสุด
// (ถ้าดึงมาหมดเป็นพันตัว กราฟจะโหลดช้าครับ)
$sql = "SELECT * FROM sensor_data ORDER BY created_at DESC LIMIT 20";
$result = $conn->query($sql);

$data = array();
if ($result->num_rows > 0) {
    while($row = $result->fetch_assoc()) {
        $data[] = $row;
    }
}

// แปลงข้อมูลเป็น JSON ส่งกลับไป
echo json_encode($data);

$conn->close();
?>