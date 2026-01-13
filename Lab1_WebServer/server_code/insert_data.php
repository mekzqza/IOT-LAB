<?php
// ตั้งค่าการเชื่อมต่อฐานข้อมูล
$servername = "localhost";
$username = "root"; // User มาตรฐานของ XAMPP
$password = "";     // Password มาตรฐานของ XAMPP (ว่างไว้)
$dbname = "iot_project";

// เชื่อมต่อ MySQL
$conn = new mysqli($servername, $username, $password, $dbname);

// เช็คว่าต่อติดไหม
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
}

// รับค่าที่ส่งมา (ใช้ $_REQUEST เพื่อรับได้ทั้ง GET และ POST)
if (isset($_REQUEST['temperature']) && isset($_REQUEST['humidity'])) {
    
    $temp = $_REQUEST['temperature'];
    $hum = $_REQUEST['humidity'];
    $name = isset($_REQUEST['sensor_name']) ? $_REQUEST['sensor_name'] : 'Unknown';

    // คำสั่ง SQL บันทึกลงตาราง
    $sql = "INSERT INTO sensor_data (sensor_name, temperature, humidity) 
            VALUES ('$name', '$temp', '$hum')";

    if ($conn->query($sql) === TRUE) {
        echo "Save OK!";
    } else {
        echo "Error: " . $sql . "<br>" . $conn->error;
    }
} else {
    echo "No data sent (temperature or humidity missing)";
}

$conn->close();
?>