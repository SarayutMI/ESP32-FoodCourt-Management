<?php
// Enable error reporting for debugging
error_reporting(E_ALL);
ini_set('display_errors', 1);

// Allow cross-origin requests
header("Access-Control-Allow-Origin: *");
header("Access-Control-Allow-Methods: POST, GET, OPTIONS");
header("Access-Control-Allow-Headers: Content-Type");
header('Content-Type: application/json');

// Database connection parameters
$servername = "localhost";
$username = "root";
$password = "180347";
$dbname = "FoodcourtSystem";

// Create a connection to the database
$conn = new mysqli($servername, $username, $password, $dbname);

// Check connection
if ($conn->connect_error) {
    die(json_encode(["success" => false, "message" => "Connection failed: " . $conn->connect_error]));
}

// Handle preflight OPTIONS request
if ($_SERVER['REQUEST_METHOD'] === 'OPTIONS') {
    exit(0);
}

// Query to fetch id, amount, and created_at from receipts table
$sql = "SELECT id, amount, created_at FROM receipts";
$result = $conn->query($sql);

// Initialize an array to store receipt information
$receipts = [];

// Check if there are results and populate the $receipts array
if ($result) {
    while ($row = $result->fetch_assoc()) {
        $receipts[] = $row;
    }

    // Return the receipt data as a JSON response
    echo json_encode(["success" => true, "data" => $receipts]);
} else {
    // Handle SQL query error
    echo json_encode(["success" => false, "message" => "Query error: " . $conn->error]);
}

// Close the database connection
$conn->close();
?>
