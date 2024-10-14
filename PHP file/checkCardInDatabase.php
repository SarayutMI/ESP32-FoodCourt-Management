<?php
error_reporting(E_ALL);
ini_set('display_errors', 1);

header("Access-Control-Allow-Origin: *"); // อนุญาตให้เข้าถึงได้จากทุกที่
header("Access-Control-Allow-Methods: GET, POST, OPTIONS"); // อนุญาตให้ใช้ GET, POST, OPTIONS
header("Access-Control-Allow-Headers: Content-Type"); // อนุญาตให้ใช้ Header ประเภท Content-Type

header('Content-Type: application/json');

$servername = "localhost";
$username = "root";
$password = "180347";
$dbname = "FoodcourtSystem";

$conn = new mysqli($servername, $username, $password, $dbname);

if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
}

if ($_SERVER['REQUEST_METHOD'] == 'POST') {
    // รับค่าจาก POST
    $card_number = $_POST['CardID']; // เปลี่ยนเป็น 'CardID' เพื่อให้ตรงกับที่ส่งจาก JavaScript

        // ใช้ Prepared Statements เพื่อป้องกัน SQL Injection
    $sql = "SELECT * FROM card_info WHERE card_number = ?";
    $stmt = $conn->prepare($sql);
    $stmt->bind_param("s", $card_number); // bind ค่า $card_number เป็นตัวแปรที่ต้องการ

    // ทำการ execute คำสั่ง
    $stmt->execute();
    $result = $stmt->get_result();

    if ($result->num_rows > 0) {
        // ถ้าพบข้อมูลบัตร
        echo json_encode(["exists" => true]);
    } else {
        // ถ้าไม่พบข้อมูลบัตร
        echo json_encode(["exists" => false]);
    }

    // ปิด statement
    $stmt->close();
}

// ปิดการเชื่อมต่อกับฐานข้อมูล
$conn->close();
?>
