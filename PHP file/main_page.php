<?php
// เริ่มเซสชัน
session_start();

// การเชื่อมต่อฐานข้อมูล
$servername = "localhost";
$dbUsername = "root";  // เปลี่ยนเป็น username ของคุณ
$dbPassword = "180347";  // เปลี่ยนเป็น password ของคุณ
$dbname = "FoodcourtSystem";  // ชื่อฐานข้อมูล

// สร้างการเชื่อมต่อกับฐานข้อมูล
$conn = new mysqli($servername, $dbUsername, $dbPassword, $dbname);

// ตรวจสอบการเชื่อมต่อ
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
}

// ตรวจสอบการรับข้อมูลจากฟอร์ม
if ($_SERVER["REQUEST_METHOD"] == "POST") {
    // รับข้อมูลจากฟอร์ม
    $username = mysqli_real_escape_string($conn, $_POST['uname']);
    $password = mysqli_real_escape_string($conn, $_POST['psw']);

    // สร้าง SQL เพื่อตรวจสอบข้อมูลผู้ใช้
    $sql = "SELECT * FROM users WHERE username='$username'";
    $result = $conn->query($sql);

    if ($result->num_rows > 0) {
        // ดึงข้อมูลผู้ใช้จากฐานข้อมูล
        $row = $result->fetch_assoc();

        // ตรวจสอบรหัสผ่าน
        if (password_verify($password, $row['password'])) {
            // เก็บข้อมูลลงในเซสชัน
            $_SESSION['username'] = $username;
            
            // เปลี่ยนเส้นทางไปยัง page2 บน ESP
            header("Location: http://172.20.10.2/page2");
            exit(); // หยุดการทำงานเพิ่มเติมเพื่อให้มั่นใจว่าถูกเปลี่ยนหน้า
        } else {
            echo "รหัสผ่านไม่ถูกต้อง";
        }
    } else {
        echo "ไม่พบผู้ใช้งานนี้";
    }
}
?>
