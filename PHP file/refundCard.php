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



// Assuming your connection and error handling is similar to the top-up script

if (isset($data['cardID']) && isset($data['amount'])) {
    $cardID = $conn->real_escape_string($data['cardID']);
    $amount = floatval($data['amount']);

    // Check if the balance is sufficient for the refund
    $result = $conn->query("SELECT balance FROM card_info WHERE card_number = '$cardID'");
    if ($result && $row = $result->fetch_assoc()) {
        $currentBalance = $row['balance'];
        if ($currentBalance < $amount) {
            echo json_encode(["success" => false, "message" => "Insufficient balance for refund."]);
            exit;
        }
    }

    // Prepare SQL for refunding
    $stmt = $conn->prepare("UPDATE card_info SET balance = balance - ? WHERE card_number = ?");
    $stmt->bind_param("ds", $amount, $cardID);
    
    // Execute and check result
    if ($stmt->execute()) {
        // Retrieve the new balance
        $result = $conn->query("SELECT balance FROM card_info WHERE card_number = '$cardID'");
        if ($result && $row = $result->fetch_assoc()) {
            $newBalance = $row['balance'];
            echo json_encode(["success" => true, "message" => "Refund successful.", "newBalance" => $newBalance]);
        } else {
            echo json_encode(["success" => false, "message" => "Could not retrieve new balance."]);
        }
    } else {
        echo json_encode(["success" => false, "message" => "Database update failed: " . $stmt->error]);
    }

    $stmt->close();
} else {
    echo json_encode(["success" => false, "message" => "Invalid input."]);
}

$conn->close();
