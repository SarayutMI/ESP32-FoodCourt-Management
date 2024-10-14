<?php
// การเชื่อมต่อฐานข้อมูล
$servername = "localhost";
$username = "root";  // เปลี่ยนเป็น username ของคุณ
$password = "180347";  // เปลี่ยนเป็น password ของคุณ
$dbname = "FoodcourtSystem";  // ชื่อฐานข้อมูล

// สร้างการเชื่อมต่อกับฐานข้อมูล
$conn = new mysqli($servername, $username, $password, $dbname);

// ตรวจสอบการเชื่อมต่อ
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
}

// กำหนดข้อมูลผู้ใช้ใหม่
$new_username = 'testuser1';  // ชื่อผู้ใช้ใหม่
$new_password = password_hash('testpassword1', PASSWORD_DEFAULT);  // แฮชรหัสผ่าน

// สร้าง SQL เพื่อเพิ่มผู้ใช้ใหม่
$sql = "INSERT INTO users (username, password) VALUES ('$new_username', '$new_password')";

// ตรวจสอบการเพิ่มผู้ใช้
if ($conn->query($sql) === TRUE) {
    echo "New user created successfully.";
} else {
    echo "Error: " . $sql . "<br>" . $conn->error;
}

// ปิดการเชื่อมต่อฐานข้อมูล
$conn->close();
?>
