<?php
error_reporting(E_ALL);
ini_set('display_errors', 1);


header("Access-Control-Allow-Origin: *"); // อนุญาตให้เข้าถึงได้จากทุกที่
header("Access-Control-Allow-Methods: GET, POST, OPTIONS"); // อนุญาตให้ใช้ GET, POST, OPTIONS
header("Access-Control-Allow-Headers: Content-Type"); // อนุญาตให้ใช้ Header ประเภท Content-Type
header('Content-Type: application/json');

$servername = "localhost"; // แก้ไขชื่อที่อยู่เซิร์ฟเวอร์
$username = "root";
$password = "180347";
$dbname = "FoodcourtSystem";

// สร้างการเชื่อมต่อ
$conn = new mysqli($servername, $username, $password, $dbname);

// Check connection
if ($conn->connect_error) {
    die(json_encode(["success" => false, "message" => "Connection failed: " . $conn->connect_error]));
}

if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    $cardID = $conn->real_escape_string($_POST['CardID']);

    // Check if card already exists
    $checkSql = "SELECT * FROM card_info WHERE card_number='$cardID'";
    $checkResult = $conn->query($checkSql);
    if ($checkResult->num_rows > 0) {
        echo json_encode(["success" => false, "message" => "Card already exists."]);
    } else {
        // Insert new card
        $insertSql = "INSERT INTO card_info (card_number, added_date, balance, last_update) VALUES ('$cardID', NOW(), 0, NOW())";
        if ($conn->query($insertSql) === TRUE) {
            echo json_encode(["success" => true, "message" => "Card saved successfully."]);
        } else {
            echo json_encode(["success" => false, "message" => "Error saving card: " . $conn->error]);
        }
    }
}

$conn->close();
?>
