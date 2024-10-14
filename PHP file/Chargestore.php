<?php
error_reporting(E_ALL);
ini_set('display_errors', 1);

header("Access-Control-Allow-Origin: *");
header("Access-Control-Allow-Methods: POST, GET, OPTIONS");
header("Access-Control-Allow-Headers: Content-Type");
header('Content-Type: application/json');

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

if ($_SERVER['REQUEST_METHOD'] === 'OPTIONS') {
    exit(0); // จบการทำงานถ้าเป็นคำขอ OPTIONS
}

// รับข้อมูล JSON
$data = json_decode(file_get_contents("php://input"), true);

if (isset($data['cardID']) && isset($data['amount'])) {
    $cardID = $conn->real_escape_string($data['cardID']);
    $amount = floatval($data['amount']);

    // ตรวจสอบว่ามีบัตรในตาราง card_info และยอดเงินเพียงพอ
    $result = $conn->query("SELECT balance FROM card_info WHERE card_number = '$cardID'");
    if ($result && $row = $result->fetch_assoc()) {
        $currentBalance = $row['balance'];
        if ($currentBalance < $amount) {
            echo json_encode(["success" => false, "message" => "ยอดเงินไม่เพียงพอ"]);
            exit;
        }

        // ทำการดึงเงินออกจากบัตร
        $stmt = $conn->prepare("UPDATE card_info SET balance = balance - ? WHERE card_number = ?");
        $stmt->bind_param("ds", $amount, $cardID);

        if ($stmt->execute()) {
            // บันทึกจำนวนเงินที่ดึงมาในตาราง receipts
            $insertReceipt = $conn->prepare("INSERT INTO receipts (amount, created_at) VALUES (?, NOW())");
            $insertReceipt->bind_param("d", $amount);

            if ($insertReceipt->execute()) {
                // ดึงยอดเงินคงเหลือใหม่หลังจากการอัปเดต
                $newBalanceResult = $conn->query("SELECT balance FROM card_info WHERE card_number = '$cardID'");
                if ($newBalanceResult && $balanceRow = $newBalanceResult->fetch_assoc()) {
                    $newBalance = $balanceRow['balance'];

                    // ส่งข้อมูลกลับในรูปแบบ JSON พร้อมยอดเงินที่ดึง และยอดเงินคงเหลือใหม่
                    echo json_encode([
                        "success" => true,
                        "message" => "ดึงเงินและบันทึกข้อมูลเรียบร้อยแล้ว",
                        "deductedAmount" => $amount,  // จำนวนเงินที่ดึงออก
                        "newBalance" => $newBalance   // ยอดเงินคงเหลือในบัตร
                    ]);
                } else {
                    echo json_encode(["success" => false, "message" => "ไม่สามารถดึงยอดเงินคงเหลือใหม่ได้"]);
                }
            } else {
                echo json_encode(["success" => false, "message" => "ไม่สามารถบันทึกข้อมูลในตาราง receipts ได้: " . $insertReceipt->error]);
            }

            $insertReceipt->close();
        } else {
            echo json_encode(["success" => false, "message" => "ไม่สามารถอัปเดตยอดเงินในบัตรได้: " . $stmt->error]);
        }

        $stmt->close();
    } else {
        echo json_encode(["success" => false, "message" => "ไม่พบข้อมูลบัตรในระบบ"]);
    }
} else {
    echo json_encode(["success" => false, "message" => "ข้อมูลอินพุตไม่ถูกต้อง"]);
}

$conn->close();
?>
