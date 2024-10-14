<?php
error_reporting(E_ALL);
ini_set('display_errors', 1);

header("Access-Control-Allow-Origin: *");
header("Access-Control-Allow-Methods: POST, GET, OPTIONS");
header("Access-Control-Allow-Headers: Content-Type");
header('Content-Type: application/json');

if ($_SERVER['REQUEST_METHOD'] == 'POST') {
    // ดำเนินการตรวจสอบความถูกต้องของการเข้าสู่ระบบที่นี่
    // ถ้าสำเร็จ ส่งกลับสถานะ "success"
    // ถ้าไม่สำเร็จ ส่งกลับสถานะ "failure"
} else {
    http_response_code(403); // ส่ง 403 ถ้าไม่ใช่ POST
    echo json_encode(['status' => 'failure', 'message' => 'Invalid request method']);
}

session_start();

$servername = "localhost";
$dbUsername = "root";
$dbPassword = "180347";
$dbname = "FoodcourtSystem";

$conn = new mysqli($servername, $dbUsername, $dbPassword, $dbname);

// ตรวจสอบการเชื่อมต่อฐานข้อมูล
if ($conn->connect_error) {
    echo json_encode(['status' => 'error', 'message' => 'เชื่อมต่อฐานข้อมูลล้มเหลว: ' . $conn->connect_error]);
    exit();
}

// ตรวจสอบวิธีการส่งข้อมูล
if ($_SERVER["REQUEST_METHOD"] == "POST") {
    if (!isset($_POST['uname']) || !isset($_POST['psw'])) {
        echo json_encode(['status' => 'error', 'message' => 'ข้อมูลการเข้าสู่ระบบไม่ครบถ้วน']);
        exit();
    }

    // รับค่าชื่อผู้ใช้และรหัสผ่าน
    $username = mysqli_real_escape_string($conn, $_POST['uname']);
    $password = mysqli_real_escape_string($conn, $_POST['psw']);

    // ค้นหาชื่อผู้ใช้ในฐานข้อมูล
    $sql = "SELECT * FROM userstore WHERE username = ?";
    $stmt = $conn->prepare($sql);
        $stmt->bind_param("s", $username);
    $stmt->execute();
    $result = $stmt->get_result();

    // ตรวจสอบผลลัพธ์ของการค้นหา
    if (!$result) {
        echo json_encode(['status' => 'error', 'message' => 'SQL error: ' . $conn->error]);
        exit();
    }

    // ตรวจสอบว่าพบชื่อผู้ใช้หรือไม่
    if ($result->num_rows > 0) {
        $row = $result->fetch_assoc();
        // ตรวจสอบรหัสผ่าน
        if (password_verify($password, $row['password'])) {
            // ถ้ารหัสผ่านถูกต้อง ให้กำหนด session
            $_SESSION['username'] = $username;
            $_SESSION['isLoggedIn'] = true;
            echo json_encode(['status' => 'success', 'isLoggedIn' => true]); // ส่งกลับว่าเป็น true
        } else {
            echo json_encode(['status' => 'error', 'message' => 'รหัสผ่านไม่ถูกต้อง']);
        }
    } else {
        echo json_encode(['status' => 'error', 'message' => 'ชื่อผู้ใช้ไม่ถูกต้อง']);
    }
}

// ปิดการเชื่อมต่อ
$conn->close();
?>
