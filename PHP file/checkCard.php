<?php
error_reporting(E_ALL);
ini_set('display_errors', 1);

header("Access-Control-Allow-Origin: *"); // อนุญาตให้เข้าถึงได้จากทุกที่
header("Access-Control-Allow-Methods: POST, GET, OPTIONS"); // อนุญาตให้ใช้ GET, POST, OPTIONS
header("Access-Control-Allow-Headers: Content-Type"); // อนุญาตให้ใช้ Header ประเภท Content-Type
header('Content-Type: application/json');

$servername = "localhost"; // แก้ไขชื่อที่อยู่เซิร์ฟเวอร์
$username = "root";
$password = "180347";
$dbname = "FoodcourtSystem";

// สร้างการเชื่อมต่อ
$conn = new mysqli($servername, $username, $password, $dbname);

// ตรวจสอบการเชื่อมต่อ
if ($conn->connect_error) {
    die(json_encode(["success" => false, "message" => "Connection failed: " . $conn->connect_error]));
}

// ตรวจสอบวิธีการ HTTP
if ($_SERVER['REQUEST_METHOD'] === 'OPTIONS') {
    exit(0); // จบการทำงานถ้าเป็นคำขอ OPTIONS
}

// รับข้อมูล JSON
$data = json_decode(file_get_contents("php://input"), true);

// ตรวจสอบว่าข้อมูลที่ได้รับมีอยู่หรือไม่
if (isset($data['cardID'])) {
    $cardID = $conn->real_escape_string($data['cardID']);

    // ตรวจสอบค่าที่ได้รับ
    if (empty($cardID)) {
        echo json_encode(["success" => false, "message" => "Invalid input."]);
        exit;
    }

    // ตรวจสอบยอดเงินปัจจุบันในบัตร
    $checkSql = "SELECT balance FROM card_info WHERE card_number = ?";
    $stmtCheck = $conn->prepare($checkSql);
    $stmtCheck->bind_param("s", $cardID);
    $stmtCheck->execute();
    $result = $stmtCheck->get_result();
    $currentBalance = 0;

    if ($result->num_rows > 0) {
        $row = $result->fetch_assoc();
        $currentBalance = $row['balance'];
        echo json_encode(["success" => true, "balance" => $currentBalance]);
    } else {
        echo json_encode(["success" => false, "message" => "Card not found."]);
        exit;
    }

    // ปิดคำสั่ง
    $stmtCheck->close();
} else {
    echo json_encode(["success" => false, "message" => "Invalid input."]);
}

// ปิดการเชื่อมต่อ
$conn->close();
?>
