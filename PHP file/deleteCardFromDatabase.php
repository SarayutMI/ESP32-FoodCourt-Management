<?php
error_reporting(E_ALL);
ini_set('display_errors', 1);

header("Access-Control-Allow-Origin: *"); // อนุญาตให้เข้าถึงได้จากทุกที่
header("Access-Control-Allow-Methods: POST, OPTIONS"); // อนุญาตให้ใช้ GET, POST, OPTIONS
header("Access-Control-Allow-Headers: Content-Type"); // อนุญาตให้ใช้ Header ประเภท Content-Type

header('Content-Type: application/json');

// กำหนดค่าการเชื่อมต่อกับฐานข้อมูล
$servername = "localhost";
$username = "root";
$password = "180347";
$dbname = "FoodcourtSystem";

// สร้างการเชื่อมต่อ
$conn = new mysqli($servername, $username, $password, $dbname);

// ตรวจสอบการเชื่อมต่อ
if ($conn->connect_error) {
    die(json_encode(["success" => false, "message" => "Connection failed: " . $conn->connect_error]));
}

if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    // รับค่าจาก POST
    $cardID = $conn->real_escape_string($_POST['cardID']);

    // ลบข้อมูลบัตรจากฐานข้อมูล
    $sql = "DELETE FROM card_info WHERE card_number='$cardID'";
    if ($conn->query($sql) === TRUE) {
        echo json_encode(["success" => true, "message" => "Card deleted successfully."]);
    } else {
        echo json_encode(["success" => false, "message" => "Error deleting card: " . $conn->error]);
    }
}

$conn->close();
?>

