<?php
error_reporting(E_ALL);
ini_set('display_errors', 1);

header("Access-Control-Allow-Origin: *");
header("Access-Control-Allow-Methods: POST, GET, OPTIONS");
header("Access-Control-Allow-Headers: Content-Type");
header('Content-Type: application/json');

// ข้อมูลการเชื่อมต่อฐานข้อมูล
$servername = "localhost";
$dbUsername = "root";
$dbPassword = "180347";
$dbname = "FoodcourtSystem";

// สร้างการเชื่อมต่อ
$conn = new mysqli($servername, $dbUsername, $dbPassword, $dbname);

// ตรวจสอบการเชื่อมต่อ
if ($conn->connect_error) {
    die(json_encode(["success" => false, "message" => "Connection failed: " . $conn->connect_error]));
}

// รับข้อมูล JSON
$data = json_decode(file_get_contents("php://input"), true);

// ตรวจสอบข้อมูลที่ได้รับ
if (isset($data['username']) && isset($data['password'])) {
    $username = $conn->real_escape_string($data['username']);
    // ใช้ password_hash() ในการแฮชรหัสผ่าน
    $hashedPassword = password_hash($data['password'], PASSWORD_DEFAULT);
    $createdAt = date('Y-m-d H:i:s');

    // ตรวจสอบว่ามีผู้ใช้ชื่อเดียวกันหรือไม่ในตาราง userstore
    $checkUserSql = "SELECT * FROM userstore WHERE username = ?";
    $stmtCheck = $conn->prepare($checkUserSql);
    $stmtCheck->bind_param("s", $username);
    $stmtCheck->execute();
    $resultCheck = $stmtCheck->get_result();

    // ตรวจสอบผลลัพธ์ว่ามีผู้ใช้ชื่อเดียวกันหรือไม่
    if ($resultCheck->num_rows > 0) {
        echo json_encode(["success" => false, "message" => "Username already exists."]);
        exit;
    }

    // บันทึกข้อมูลผู้ใช้ใหม่ในตาราง userstore
    $insertSql = "INSERT INTO userstore (username, password, created_at) VALUES (?, ?, ?)";
    $stmtInsert = $conn->prepare($insertSql);
    $stmtInsert->bind_param("sss", $username, $hashedPassword, $createdAt);

    if ($stmtInsert->execute()) {
        echo json_encode(["success" => true, "message" => "User registered successfully."]);
    } else {
        echo json_encode(["success" => false, "message" => "Error: " . $stmtInsert->error]);
    }

    // ปิดคำสั่ง
    $stmtInsert->close();
    $stmtCheck->close();
} else {
    echo json_encode(["success" => false, "message" => "Invalid input."]);
}

// ปิดการเชื่อมต่อฐานข้อมูล
$conn->close();
?>
