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
$dbname = "FoodcourtSystem"; // สร้างการเชื่อมต่อ 
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
if (isset($data['cardID']) && isset($data['amount'])) {
    $cardID = $conn->real_escape_string($data['cardID']);
    $amount = floatval($data['amount']); // แปลงเป็นจำนวนทศนิยม

    // ตรวจสอบค่าที่ได้รับ
    if (empty($cardID) || !is_numeric($amount)) {
        echo json_encode(["success" => false, "message" => "Invalid input."]);
        exit;
    }

    // เตรียม SQL สำหรับการอัปเดตยอดเงิน 
    $stmt = $conn->prepare("UPDATE card_info SET balance = balance + ? WHERE card_number = ?");
    if (!$stmt) {
        echo json_encode(["success" => false, "message" => "SQL statement preparation failed: " . $conn->error]);
        exit;
    }

    $stmt->bind_param("ds", $amount, $cardID); // "d" สำหรับจำนวนทศนิยมและ "s" สำหรับสตริง

    // ดำเนินการคำสั่ง
    if ($stmt->execute()) {
        // Retrieve the new balance after the update
        $result = $conn->query("SELECT balance FROM card_info WHERE card_number = '$cardID'");
        if ($result && $row = $result->fetch_assoc()) {
            $newBalance = $row['balance'];
            echo json_encode(["success" => true, "message" => "Top-up successful.", "newBalance" => $newBalance]);
        } else {
            echo json_encode(["success" => false, "message" => "Could not retrieve new balance."]);
        }
    } else {
        echo json_encode(["success" => false, "message" => "Database update failed: " . $stmt->error]);
    }

    // ปิดคำสั่ง
    $stmt->close();
} else {
    echo json_encode(["success" => false, "message" => "Invalid input."]);
}

// ปิดการเชื่อมต่อ
$conn->close();
?>
