<?php
error_reporting(E_ALL);
ini_set('display_errors', 1);

header("Access-Control-Allow-Origin: *"); // อนุญาตให้เข้าถึงได้จากทุกที่
header("Access-Control-Allow-Methods: POST, GET, OPTIONS"); // อนุญาตให้ใช้ GET, POST, OPTIONS
header("Access-Control-Allow-Headers: Content-Type"); // อนุญาตให้ใช้ Header ประเภท Content-Type
header('Content-Type: application/json');

// ข้อมูลการเชื่อมต่อฐานข้อมูล
$servername = "localhost"; // ที่อยู่เซิร์ฟเวอร์
$username = "root"; // ชื่อผู้ใช้ฐานข้อมูล
$password = "180347"; // รหัสผ่านฐานข้อมูล
$dbname = "FoodcourtSystem"; // ชื่อฐานข้อมูล

// สร้างการเชื่อมต่อ
$conn = new mysqli($servername, $username, $password, $dbname);

// ตรวจสอบการเชื่อมต่อ
if ($conn->connect_error) {
    die(json_encode(array("status" => "error", "message" => "Connection failed: " . $conn->connect_error)));
}

// ตรวจสอบว่าเป็นการร้องขอด้วยวิธี POST
if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    // รับค่าจากการส่งข้อมูล
    $username = $_POST['regUsername'];
    $password = $_POST['regPassword'];

    // ตรวจสอบว่า username มีอยู่ในระบบหรือไม่
    $checkUsername = $conn->prepare("SELECT * FROM users WHERE username = ?");
    $checkUsername->bind_param("s", $username);
    $checkUsername->execute();
    $result = $checkUsername->get_result();

    if ($result->num_rows > 0) {
        // หากชื่อผู้ใช้มีอยู่แล้ว
        echo json_encode(array("status" => "error", "message" => "Username already exists. Please choose another."));
    } else {
        // แทรกข้อมูลลงในตาราง users
        $stmt = $conn->prepare("INSERT INTO users (username, password, created_at) VALUES (?, ?, NOW())");
        if ($stmt) {
            $stmt->bind_param("ss", $username, $password); // ข้อมูล username และ password
            // ส่งคำสั่ง SQL และตรวจสอบผลลัพธ์
            if ($stmt->execute()) {
                echo json_encode(array("status" => "success", "message" => "Registration successful!"));
            } else {
                echo json_encode(array("status" => "error", "message" => "Error executing query: " . $stmt->error)); // แสดงข้อผิดพลาด
            }
            $stmt->close();
        } else {
            echo json_encode(array("status" => "error", "message" => "Error preparing statement: " . $conn->error)); // แสดงข้อผิดพลาด
        }
    }

    $checkUsername->close();
}

// ปิดการเชื่อมต่อฐานข้อมูล
$conn->close();
?>
