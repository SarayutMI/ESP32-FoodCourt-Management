#include <FS.h>
#include <SPIFFS.h>
#include <WiFi.h>
#include <WiFiManager.h>
#include <DNSServer.h>
#include <WebServer.h>
#include <SPI.h>
#include <MFRC522.h>
#include <HTTPClient.h>
#include <ArduinoJson.h> // Include ArduinoJson for JSON parsing
#include <LiquidCrystal_I2C.h>

const char* datatable = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Receipt Information</title>
    <style>
      * {
        margin: 0;
        padding: 0;
        box-sizing: border-box;
        font-family: 'Bebas Neue', sans-serif;
        }
      body {
        background-color: #f4f4f4;
        padding: 20px;
      }
      h1 {
        text-align: center;
        margin-bottom: 20px;
        font-size: 40px;
      }
      table {
        width: 100%;
        border-collapse: collapse;
        margin-bottom: 20px;
        font-size: 20px;
      }
      table, th, td {
        border: 1px solid black;
      }
      th, td {
        padding: 10px;
        text-align: center;
      }
        .button-container {
            display: flex; /* Use flexbox to align items in a row */
            justify-content: center; /* Center the buttons */
            margin-bottom: 20px; /* Add some space below the buttons */
        }

        button {
            padding: 10px 20px; /* Add some padding */
            border: none; /* Remove default border */
            cursor: pointer; /* Change cursor to pointer */
            font-size: 18px; /* Set font size */
            margin: 0 10px; /* Add space between buttons */
            color: white; /* Text color */
            border-radius: 5px;
        }

        .refresh-btn {
            background-color: #F66B00; /* Refresh button color */
        }

        .back-btn {
            background-color: gray; /* Back button color */
        }

        button:hover {
            opacity: 0.9; /* Slightly change opacity on hover */
        }
        .total-amount {
            font-weight: bold;
            text-align: right;
            font-size: 30px;
            margin: 20px;
        }
    </style>
</head>
<body>
    <h1>Receipt Information</h1>
    <table>
      <thead>
        <tr>
          <th>ID</th>
          <th>Amount</th>
          <th>Update At</th>
        </tr>
      </thead>
      <tbody id="receiptTableBody">
        <!-- Data will be injected here by JavaScript -->
      </tbody>
    </table>
    <div class="total-amount" id="totalAmount">
      Total Amount: 0
    </div>
    <div class="button-container">
      <button class="refresh-btn" onclick="refreshData()">Refresh</button>
      <button class="back-btn" onclick="window.location.href='/page2?isLoggedIn=true'">Back</button>
    </div>
    
    <script>
      const serverIP = "%SERVERIP%"; // Replace this with the actual server IP when deploying

      // Function to fetch and display data from receipts table
      function refreshData() {
        fetch('http://' + serverIP + '/senddatatotableReceipts.php') // Update the PHP file path if needed
        .then(response => response.json())
        .then(data => {
          if (!data.success) {
              console.error('Error fetching data:', data.message);
              alert('Error: ' + data.message);
              return;
          }

          let tableBody = document.getElementById('receiptTableBody');
          tableBody.innerHTML = ''; // Clear old data
          let totalAmount = 0; // Variable to store total amount

          // Ensure that data.data contains an array of receipt information
          data.data.forEach(row => {
              let tr = document.createElement('tr');
              tr.innerHTML = `<td>${row.id}</td><td>${row.amount}</td><td>${row.created_at}</td>`;
              tableBody.appendChild(tr);

              // Calculate the total amount by adding the amount from each row
              totalAmount += parseFloat(row.amount);
          });

          // Display the total amount at the bottom of the table
          document.getElementById('totalAmount').textContent = 'Total Amount: ' + totalAmount.toFixed(2);
        })
        .catch(error => {
          console.error('Error:', error);
        });
      }

      // Call the function to load data when the page is loaded
      window.onload = refreshData;
    </script>
</body>
</html>
)rawliteral";


const char* htmlContent = R"rawliteral(
<!DOCTYPE html>
<html>
  <head>
    <meta charset="utf-8" />
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <link href="https://fonts.googleapis.com/css2?family=Bebas+Neue&display=swap" rel="stylesheet">
    <style>
      /* Styles */
      * {
        margin: 0;
        padding: 0;
        box-sizing: border-box;
        font-family: 'Bebas Neue', sans-serif;
      }
      .Top-title {
        height: 150px;
        margin: 0 auto;
        color: #312f2f;
        font-size: 100px;
        text-align: center;
        position: relative;
        top: 100px;
        left: 50%;
        transform: translate(-50%, -50%);
      }
      .ip-address {
        font-size: 20px;
        text-align: center;
        margin-top: 20px;
      }
      .logintitle {
        height: 60px;
        margin: 0 auto;
        color: #000000;
        font-size: 50px;
        text-align: center;
        position: relative;
        top: 50px;
        left: 50%;
        transform: translate(-50%, -50%);
      }
      .from {
        height: 240px;
        margin: 0 auto;
        font-size: 25px;
        text-align: center;
        position: relative;
        top: 200px;
        left: 50%;
        transform: translate(-50%, -50%);
      }
      .container input[type="text"],
      .container input[type="password"] {
        width: 300px;
        padding: 10px;
        border: 2px solid #ddd;
        border-radius: 5px;
        font-size: 16px;
        margin-top: 10px;
      }
      .button-container {
        justify-content: center;
        gap: 10px;
        position: fixed;
        top: 180px;
        left: 50%;
        transform: translateX(-50%);
      }
      .clearbtn, .submitbtn {
        background-color: #636363;
        border: none;
        color: white;
        padding: 10px 30px;
        border-radius: 15px;
        font-size: 20px;
      }
      .submitbtn {
        background-color: #F66B00;
      }
      .clearbtn:hover {
        background-color: #353535;
      }
      .submitbtn:hover {
        background-color: #ce5900;
      }
      button[type="submit"] {
        background-color: #F66B00;
        border: none;
        color: white;
        padding: 10px 30px;
        border-radius: 15px;
        font-size: 20px;
      }
      button[type="submit"]:hover {
        background-color: rgb(214, 96, 0);
        transform: scale(1.1);
      }
    </style>
</head>
<body>
    <div class="Top-title">
        <b>Food Court System | Store</b>
    </div>
    <div class="ip-address">
        %IP_ADDRESS%
    </div>
    <div class="login">
        <div class="logintitle">
            <p>LOGIN</p>
            <div class="from">
                <form id="myForm">
                    <div class="container">
                        <label for="uname"><b>Username </b></label>
                        <input type="text" id="uname" placeholder="Enter Username" name="uname" required>
                        <br>
                        <label for="psw"><b>Password </b></label>
                        <input type="password" id="psw" placeholder="Enter Password" name="psw" required>
                        <br><br>
                    </div>
                    <div class="button-container">
                        <button type="button" class="clearbtn" onclick="resetForm()">Clear</button>
                        <button type="submit" class="submitbtn">Login</button>
                    </div>
                </form>
                <div id="responseMessage"></div>
                  <script>
                      const serverIP = "%SERVERIP%"  // แทรกตัวแปร IP จาก C++
                      // Reset form
                      function resetForm() {
                          document.getElementById("myForm").reset();
                      }

                      // Handle form submission
                      document.getElementById("myForm").addEventListener("submit", function(event) {
                          event.preventDefault(); // Prevent default form submission

                          // Get values from the form
                          const username = document.getElementById("uname").value;
                          const password = document.getElementById("psw").value;

                          fetch('http://' + serverIP + '/loginstore.php', {
                            method: "POST",
                            headers: {
                                "Content-Type": "application/x-www-form-urlencoded",
                            },
                            body: `uname=${encodeURIComponent(username)}&psw=${encodeURIComponent(password)}`,
                        })
                        .then(response => {
                            if (!response.ok) {
                                throw new Error('Network response was not ok');
                            }
                            return response.json(); // Convert response to JSON
                        })
                        .then(data => {
                            if (data.status === 'success') {
                              window.location.href = '/page2?isLoggedIn=' + data.isLoggedIn; 
                            } else {
                                // If login failed, show message
                                document.getElementById("responseMessage").innerText = data.message || "Login failed. Please try again.";
                            }
                        })
                        .catch(error => {
                            console.error("Error occurred:", error);
                            document.getElementById("responseMessage").innerText = "Connection error occurred.";
                        });

                      });
                  </script>
            </div>
        </div>
    </div>
</body>

</html>
)rawliteral";

// HTML for the second page (after successful login)
const char* httpResponseCode = R"rawliteral(
<!DOCTYPE html>
<html>
  <head>
    <meta charset="utf-8" />
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <link href="https://fonts.googleapis.com/css2?family=Bebas+Neue&display=swap" rel="stylesheet">
<style>
    * {
    margin: 0;
    padding: 0;
    box-sizing: border-box; /* แก้ไขจาก 0 เป็น border-box */
    font-family: 'Bebas Neue', sans-serif;
}
.modaladdcard input {
    font-size: 20px; /* ขนาดตัวอักษรของ input */
    padding: 10px; /* ระยะห่างภายในของ input */
    width: 30%; /* ความกว้าง 100% ของ input */
    box-sizing: border-box; /* ให้ padding รวมในความกว้าง */
    border: 1px solid #ccc;
    border-radius: 5px;
    background-color: #ffffff;
    box-shadow: 0px 2px 4px rgba(0, 0, 0, 0.1);
}
.titlesite{
    height: 60px;
    background-color: #ffffff;
    margin: 0 auto;
    color: #000000;
    font-size: 50px;
    text-align: center;
    position: absolute;
    top: 10px;
    left: 10px;
}
.Addcardbtn{
    border: none;
    color: rgb(255, 255, 255);
    background-color: #ff8c00;
    padding: 10px 30px;
    border-radius: 15px;
    font-size: 20px;
    cursor: pointer;
    position: absolute;
    top: 10px;
    right: 270px;
}
.btn_register{
    border: none;
    color: rgb(255, 255, 255);
    background-color: #ff8c00;
    padding: 10px 30px;
    border-radius: 15px;
    font-size: 20px;
    cursor: pointer;
    position: absolute;
    top: 10px;
    right: 405px;
}
.Dashbordbtn{
    border: none;
    color: rgb(255, 255, 255);
    background-color: #ff8c00;
    padding: 10px 30px;
    border-radius: 15px;
    font-size: 20px;
    cursor: pointer;
    position: absolute;
    top: 10px;
    right: 130px;
}
.LOGOUTbtn{
    border: none;
    color: white;
    background-color: #ff8c00;
    padding: 10px 30px;
    border-radius: 15px;
    font-size: 20px;
    cursor: pointer;
    position: absolute;
    top: 10px;
    right: 10px;
}

.btn_register:hover, .Dashbordbtn:hover, .LOGOUTbtn:hover, .Addcardbtn:hover {
    background-color: #973c00;
    text-decoration: none;
    transform: scale(1.1);
    color: white;
    transition: background-color 0.3s ease, transform 0.3s ease;
}

hr.solid {
    border: none; 
    border-top: 3px solid #d45500;
    margin: 70px 0; 
    width: 100%; 
    align-self: center; 
}
.inputcard{
    height: 70px;
    margin: 0 auto;
    font-size: 45px;
    text-align: center;
    position: absolute;
    top: 15%;
    left: 17%;
}
#CardID {
    width: 300px;
    font-size:30px;
    padding: 5px;
    border: 1px solid #ccc;
    border-radius: 5px;
    background-color: #ffffff;
    box-shadow: 0px 2px 4px rgba(0, 0, 0, 0.1);
}
.inputcard input[type="text"]{
    width: 500px;
    padding: 5px;
    border: 2px solid #ddd;
    border-radius: 5px;
    font-size: 25px;
    box-sizing: border-box;
    text-align: center;
    transition: border-color 0.3s, box-shadow 0.3s;
    margin-top: 10px;
}
input[type="text"], input[type="password"] {
    width: 30%; /* ความกว้างเต็มที่ */
    font-size: 20px;
    padding: 5px; /* Padding รอบ ๆ */
    margin: 5px 0 10px; /* ระยะห่าง */
    border: 1px solid #ccc; /* ขอบของช่องกรอก */
    border-radius: 5px; /* ทำมุมมน */
    box-sizing: border-box; /* รวม Padding และ Border ในความกว้าง */
}

.inputcard input::placeholder {
    color: #aaa;
    font-style: italic;
}

label[for="balance"] {
    font-size: 40px;
    color: #000000;
    margin-bottom: 10px;
    display: block;
    position: absolute;
    top: 42%;
    left: 19%;
}

#balance-display {
    margin: 0 auto;
    font-size: 70px;
    text-align: center;
    position: absolute;
    top: 50%;
    left: 15%;
    background-color: none;
    padding: 10px;
    border-radius: 5px;
    width: 350px;
    height: 120px;
    border: 2px solid #ddd;
}

#balance-display input[type="number"] {
    width: 100%;
    height: 150px;
    padding: 10px 50px 10px 10px;
    border: 2px solid #ddd;
    border-radius: 5px;
    font-size: 80px;
    box-sizing: border-box;
    text-align: center;
    transition: border-color 0.3s, box-shadow 0.3s;
    margin-top: 10px;
    position: relative;
}

#balance-display::after {
    content: 'THB';
    position: absolute;
    right: 10px;
    top: 50%;
    transform: translateY(-50%);
    font-size: 30px;
    color: #333;
}

.calculator {
    display: grid;
    grid-template-columns: repeat(3, 90px);
    grid-gap: 20px;
    justify-content: center;
    margin-top: 50px;
    position: absolute;
    right: 20%;
    bottom: 10%;
    justify-content: end;
    align-content: end;
}
.btn {
    width: 90px;
    height: 90px;
    font-size: 50px;
    background-color: #ffffff;
    border: 2px solid #ddd;
    border-radius: 5px;
    box-shadow: 0 4px 6px rgba(0, 0, 0, 0.1);
    cursor: pointer;
    transition: background-color 0.3s, box-shadow 0.3s;
    display: flex;
    justify-content: center;
    align-items: center;
    padding: 0;
}
.btn:hover {
    background-color: #f7f7f7;
    box-shadow: 0 6px 8px rgba(0, 0, 0, 0.2);
}
.zero {
    grid-column: span 3;
}
.btn-remove, .btn-clear{
    width: 90px;
    height: 90px;
    font-size: 30px;
    background-color: #ffffff;
    border: 2px solid #ddd;
    border-radius: 5px;
    box-shadow: 0 4px 6px rgba(0, 0, 0, 0.1);
    cursor: pointer;
    transition: background-color 0.3s, box-shadow 0.3s;
    display: flex;
    justify-content: center;
    align-items: center;
    padding: 0;
}
.btn-remove{
    color: #d45500;
}
#display {
    font-size: 40px;
    margin-top: 20px;
    padding: 10px;
    width: 350px;
    text-align: right;
    border: 2px solid #ddd;
    border-radius: 5px;
    background-color: #fff;
}
.refund-btn ,.check-btn {
    border: none;
    color: white;
    background-color: #949494;
    padding: 10px 50px;
    border-radius: 15px;
    font-size: 40px;
    cursor: pointer;
    position: absolute;
    top: 20%;
    transition: background-color 0.3s, transform 0.3s;
}
.refund-btn:hover ,.check-btn:hover {
    background-color: #d45500;
}

#refund-btn {
    left: 55%;
    top:15%;
}

#check-btn {
    left: 72%;
    top:15%;
}

.modaladdcard ,.modalregister{
    font-size: 30px;
    display: none;
    position: fixed;
    text-align: center;
    display: none; /* ซ่อน modal โดยค่าเริ่มต้น */
    position: fixed;
    z-index: 1;
    left: 0;
    top: 0;
    width: 100%;
    height: 100%;
    overflow: auto;
    background-color: rgb(0,0,0);
    background-color: rgba(0,0,0,0.4);
}

/* สไตล์เนื้อหา modal */
.modaladdcard-content{
    background-color: #fefefe;
    margin: 15% auto;
    padding: 10px;
    border: 1px solid #888;
    width: 40%;
    height: 40%;
    border-radius: 8px; /* ขอบโค้ง */
}
.modalregister-content{
    background-color: #fefefe;
    margin: 15% auto;
    padding: 10px;
    border: 1px solid #888;
    width: 50%;
    height: 50%;
    border-radius: 8px; /* ขอบโค้ง */
}
/* สไตล์ปุ่มปิด */
.close {
    color: #aaa;
    float: right;
    font-size: 28px;
    font-weight: bold;
}
/* ปุ่ม Scan */
.btn-scan {
    background-color: #210085; 
    color: white;
    width: 150px;
}

/* ปุ่ม Save */
.btn-save {
    background-color: #60c502; 
    color: white;
    top: 30%;

}

/* ปุ่ม Delete */
.btn-delete {
    background-color: #ff1100;
    color: white;
}

/* ปุ่ม Back */
.btn-back {
    background-color: #757575; /* สีเทา */
    color: black;
}

.btn-scan, .btn-save, .btn-delete, .btn-back{
    border: none; /* ไม่ให้มีขอบ */
    color: white; /* สีตัวอักษร */
    padding: 10px 20px; /* ขนาดของปุ่ม */
    border-radius: 5px; /* มุมโค้งของปุ่ม */
    font-family: "Bebas Neue Regular", sans-serif; /* ใช้ฟอนต์ที่ต้องการ */
    font-size: 20px; /* ขนาดตัวอักษร */
    cursor: pointer; /* เปลี่ยนเคอร์เซอร์เป็นมือเมื่อ hover */
    margin: 5px; /* ระยะห่างระหว่างปุ่ม */
    transition: background-color 0.3s, transform 0.3s; /* การเปลี่ยนแปลงเมื่อ hover */
}

@media (min-width: 768px) and (max-width: 820px) and (orientation: portrait){
    .btn_register, .Addcardbtn, .Dashbordbtn, .LOGOUTbtn{
        margin: 0 2;
        font-size: 20px;
        text-align: center;
        position: absolute;
        padding: 10px 15px;
    }
    .btn_register{
        right: 320px;
    }
    .Addcardbtn{
        right: 210px;
    }
    .Dashbordbtn{
        right: 100px;
    }
    .LOGOUTbtn{
        right: 10px;
    }

    .inputcard{
        top: 10%;
        left: 50%;
        transform: translate(-50%, -50%);
    }
    .inputcard input[type="text"]{
        width: 500px;
        font-size: 15px;
    }

    label[for="balance"] {
        font-size: 30px; /* ขนาดของฟอนต์ */
        top: 20%;
        left: 50%;
        transform: translate(-50%,-50%);
    }
    #balance-display{
        font: 50;
        top: 27%;
        left: 50%;
        transform: translate(-50%,-50%);
    }
    
    #refund-btn {
        left: 38%;  /* ตำแหน่งของปุ่ม refund */
        top: 40%;
    }
    
    #check-btn {
        left: 68%;  /* ตำแหน่งของปุ่ม check */
        top: 40%;
    }

    .calculator {
        display: grid;
        grid-template-columns: repeat(3, 90px); /* 3 คอลัมน์, ความกว้างแต่ละคอลัมน์ 90px */
        grid-gap: 20px; /* ลดช่องว่างระหว่างปุ่ม */
        justify-content: center; /* จัดให้อยู่ตรงกลาง */
        margin-top: 50px; /* เพิ่มระยะห่างจากด้านบน */
        position: absolute;
        left: 50%; /* จัดให้อยู่ตรงกลางในแนวนอน */
        bottom: 10%; /* วางห่างจากขอบล่างของหน้าจอ */
        transform: translateX(-50%); /* จัดให้อยู่กึ่งกลางในแนวนอน */
        align-content: end; /* จัดปุ่มให้อยู่ที่ด้านล่าง */
    }
    .btn-remove, .btn-clear{
        width: 90px;
        height: 90px;
        font-size: 30px;
    }
}    

/* Media query for screens between 820px and 1180px in landscape orientation */
@media (min-width: 820px) and (max-width: 1180px) and (orientation: landscape) {
    .titlesite {
        font-size: 40px; /* Adjust font size for better fit */
        height: 50px; /* Reduce height */
        top: 10px;
        left: 20px; /* Adjust position */
    }
    
    .inputcard {
        font-size: 35px; /* Adjust font size */
        top: 20%; /* Adjust position */
        left: 15%; /* Adjust position */
    }

    .inputcard input[type="text"] {
        width: 400px; /* Adjust width */
        font-size: 20px; /* Adjust font size */
    }
    
    label[for="balance"] {
        font-size: 30px; /* Adjust font size */
        top: 45%; /* Adjust position */
        left: 20%; /* Adjust position */
    }
    
    #balance-display {
        font-size: 50px; /* Adjust font size */
        width: 300px; /* Adjust width */
        height: 100px; /* Adjust height */
    }
    
    #balance-display input[type="number"] {
        font-size: 50px; /* Adjust font size */
        height: 100px; /* Adjust height */
    }
    
    .calculator {
        grid-template-columns: repeat(3, 70px); /* Adjust grid columns */
        grid-gap: 15px; /* Adjust gap */
        right: 22%; /* Adjust position */
        bottom: 25%; /* Adjust position */
    }
    .refund-btn ,.check-btn {
        border: none;
        color: white;
        background-color: #949494;
        padding: 10px 40px;
        border-radius: 15px;
        font-size: 40px;
        cursor: pointer;
        position: absolute;
        top: 20%;
        transition: background-color 0.3s, transform 0.3s;
    }
    .refund-btn:hover ,.check-btn:hover {
        background-color: #d45500;
    }
    .btn {
        width: 70px; /* Adjust button size */
        height: 70px;
        font-size: 40px; /* Adjust font size */
    }
    
    #refund-btn {
        left: 60%; /* Adjust position */
        top: 20%;
    }
    
    #check-btn {
        left: 77%; /* Adjust position */
        top: 20%;
    }
    .btn-remove, .btn-clear{
        width: 70px;
        height: 70px;
        font-size: 25px;
    }
}
</style>
  </head>
<body>
    <div class="top-bar">
        <div class="titlesite">
            <p>Food Court System | store</p>
        </div>
        <div class="top-bar-buttons">
            <button class="btn_register" type="button" onclick="openRegisterModal()"> Register </button>
            <button class="Addcardbtn" type="button" onclick="OpenCardModal()">Add Card</button>
            <button class="Dashbordbtn" type="button" onclick="window.location.href='/dashbord';">Dashbord</button>
            <button class="LOGOUTbtn" type="button" onclick="confirmLogout()">LOGOUT</button>
        </div> 
    </div>
    <hr class="solid">
    <form>
        <div class="inputcard">
            <label for="CardID">Card ID</label><br>
            <input type="text" id="CardID" name="CardID" readonly>
        </div>
        <br>
        <div class="Show-balance">
            <label for="balance">AVAILABLE BALANCE</label><br>
            <div id="balance-display">0.0</div> <!-- แสดงผลยอดเงิน -->
        </div>        
    </form>

    <div class="calculator">
        <button class="btn" data-value="7">7</button>
        <button class="btn" data-value="8">8</button>
        <button class="btn" data-value="9">9</button>
        <button class="btn" data-value="4">4</button>
        <button class="btn" data-value="5">5</button>
        <button class="btn" data-value="6">6</button>
        <button class="btn" data-value="1">1</button>
        <button class="btn" data-value="2">2</button>
        <button class="btn" data-value="3">3</button>
        <button class="btn-clear" data-value="C">Clear</button>
        <button class="btn" data-value="0">0</button>
        <button class="btn-remove" data-value="R">Remove</button>
    </div>

    <button id="refund-btn" class="refund-btn" onclick="refund()">Charge</button>
    <button id="check-btn" class="check-btn" onclick="Check()">Check</button>

    <!-- Register Modal -->
    <div id="registerModal" class="modalregister">
        <div class="modalregister-content">
            <span class="close" onclick="closeRegisterModal()">&times;</span>
            <h2>Register</h2>
            <form id="registerForm" onsubmit="return validateRegister()">
                <label for="regUsername">Username:</label>
                <input type="text" id="regUsername" name="regUsername" required>
                <br>
                <label for="regPassword">Password:</label>
                <input type="password" id="regPassword" name="regPassword" required>
                <br>
                <label for="confirmRegPassword">Confirm Password:</label>
                <input type="password" id="confirmRegPassword" name="confirmRegPassword" required>
                <br><br>
                <button type="submit" class="btn-save">Register</button>
            </form>
        </div>
    </div>

    <!-- Modal สำหรับการเพิ่มบัตร -->
    <div id="cardModal" class="modaladdcard">
        <div class="modaladdcard-content">
            <span class="close" onclick="closeCardModal()">&times;</span>
            <h2>Add card</h2>

            <form id="cardForm">
                <input type="text" id="cardIDInput" name="cardIDInput" readonly>
                <br><br>

                <div class="Modalbtn">
                    <button type="button" class="btn-save" onclick="saveCard()">Save</button>
                    <button type="button" class="btn-delete" onclick="deleteCard()">Delete</button>
                    <button type="button" class="btn-back" onclick="goBack()">Back</button>
                </div>                
            </form>
        </div>
    </div>
<script>

    const serverIP = "%SERVERIP%"  // แทรกตัวแปร IP จาก C++
    
    function confirmLogout() {
        var confirmation = confirm("Are you sure you want to log out?");
        if (confirmation) {
            // ถ้าผู้ใช้กด OK
            isLoggedIn = false;
            window.location.href = '/';
        } else {
            // ถ้าผู้ใช้กด Cancel
            console.log("Logout cancelled.");
        }
    }
    function closeCardModal() {
        document.getElementById('cardModal').style.display = 'none';
    }

    function OpenCardModal() {
        document.getElementById('cardModal').style.display = 'block';
    }

    function closeRegisterModal() {
        document.getElementById('registerModal').style.display = 'none';
    }

    function openRegisterModal() {
        document.getElementById("registerModal").style.display = "block";
    }

    function goBack() {
        closeCardModal();
    }

    function updateCardID(cardID) {
        document.getElementById('CardID').value = cardID; // Display in the input field
        document.getElementById('cardIDInput').value = cardID; // Display in modal input
        console.log('Card ID updated to: ' + cardID); // Debugging
    }

    // ฟังก์ชันสำหรับดึงเลขบัตรทุก 1 วินาที
    setInterval(() => {
        fetch('/sendCardID')
            .then(response => response.text())
            .then(cardID => {
                console.log('Received card ID:', cardID); // Debugging line
                if (cardID && cardID !== "No card detected") {
                    updateCardID(cardID); // แสดงเลขบัตรใน input field
                }
            })
            .catch(error => {
                console.error('Error fetching card ID:', error);
            });
    }, 1000); // ทุก 1 วินาที

    function deleteCard() {
    const cardID = document.getElementById('cardIDInput'); // เปลี่ยน 'cardID' เป็น 'cardIDInput'
    if (!cardID) {
        alert('Card ID input not found.');
        return;
    }

    const cardIDValue = cardID.value; // รับค่าจาก input
    if (!cardIDValue) {
        alert('Please enter a Card ID to delete.');
        return;
    }

    // ทำการเรียก fetch ไปยัง checkCard.php เพื่อตรวจสอบจำนวนเงินที่เหลือ
    fetch('http://' + serverIP + '/checkCard.php', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json',
        },
        body: JSON.stringify({ cardID: cardIDValue }) // ส่งข้อมูลในรูปแบบ JSON
    })
    .then(response => response.json())
    .then(balanceResult => {
        if (balanceResult.success) {
            const remainingBalance = balanceResult.balance; // รับค่าจำนวนเงินที่เหลือ
            const confirmMessage = `Are you sure you want to delete this card? Remaining balance: ${remainingBalance} Baht.`;
            
            // ถามผู้ใช้ว่าแน่ใจหรือไม่
            if (confirm(confirmMessage)) {
                // ทำการเรียก fetch ไปยัง deleteCardFromDatabase.php
                return fetch('http://' + serverIP + '/deleteCardFromDatabase.php', {
                    method: 'POST',
                    headers: {
                        'Content-Type': 'application/x-www-form-urlencoded',
                    },
                    body: `cardID=${encodeURIComponent(cardIDValue)}`
                });
            } else {
                alert('Card deletion canceled.');
                return Promise.reject('Card deletion canceled.'); // ยกเลิกการดำเนินการ
            }
        } else {
            alert(balanceResult.message);
            return Promise.reject('Failed to get card balance.');
        }
    })
    .then(deleteResponse => {
        return deleteResponse.json();
    })
    .then(result => {
        if (result.success) {
            alert(result.message);
        } else {
            alert(result.message);
        }
    })
    .catch(error => {
        console.error('Error:', error);
        alert('An error occurred while deleting the card.');
    });
}
    function saveCard() {
        const cardID = document.getElementById('cardIDInput').value; // ใช้ cardIDInput ใน modal
        const body = `cardID=${encodeURIComponent(cardID)}`;
        if (!cardID) {
            alert('Please scan a card before saving.');
            return;
        }

        // ตรวจสอบว่าบัตรมีอยู่ในฐานข้อมูล
        fetch('http://' + serverIP + '/checkCardInDatabase.php', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/x-www-form-urlencoded',
            },
            body: `CardID=${encodeURIComponent(cardID)}`
        })
        .then(response => response.json())
        .then(result => {
            if (result.exists) {
                alert('Card already exists in the database.');
            } else {
                // ถ้าบัตรไม่มีอยู่ในฐานข้อมูล บันทึกบัตรใหม่
                fetch('http://' + serverIP + '/addCardToDatabase.php', {
                    method: 'POST',
                    headers: {
                        'Content-Type': 'application/x-www-form-urlencoded',
                    },
                    body: `CardID=${encodeURIComponent(cardID)}`
                })
                .then(saveResponse => saveResponse.text())
                .then(saveResult => {
                    alert('Card saved successfully.');
                    closeCardModal();
                })
                .catch(saveError => {
                    console.error('Error saving card:', saveError);
                    alert('An error occurred while saving the card.');
                });
            }
        })
        .catch(error => {
            console.error('Error checking card:', error);
            alert('An error occurred while checking the card.');
        });
    }
    
    function Check() {
        // รับหมายเลขบัตรจาก input
        var cardID = document.getElementById('CardID').value; // เลขบัตร

        // ตรวจสอบว่ามีการกรอกหมายเลขบัตรหรือไม่
        if (!cardID) {
            console.error('Invalid card ID');
            alert('Please enter a valid card ID.');
            return; // ออกจากฟังก์ชันถ้าข้อมูลไม่ถูกต้อง
        }

        // สร้าง object สำหรับข้อมูลที่จะส่ง
        var data = {
            cardID: cardID
        };

        // ส่งคำขอ POST ไปยัง checkCard.php
        fetch('http://' + serverIP + '/checkCard.php', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json', // ใช้ JSON แทน
            },
            body: JSON.stringify(data) // แปลงข้อมูลเป็น JSON
        })
        .then(response => response.json()) // แปลงผลลัพธ์ที่ได้เป็น JSON
        .then(result => {
            // แสดงผลลัพธ์ที่ได้รับจาก server
            console.log(result);
            if (result.success) {
                // ถ้ายอดเงินถูกต้อง จะแสดงยอดเงิน
                alert('Card balance: ' + result.balance);
            } else {
                alert('Error: ' + result.message);
            }
        })
        .catch(error => {
            console.error('Error:', error);
            alert('An error occurred while checking the card balance.');
        });
    }

    function validateRegister() {
        var password = document.getElementById("regPassword").value;
        var confirmPassword = document.getElementById("confirmRegPassword").value;

        // ตรวจสอบว่ารหัสผ่านตรงกันหรือไม่
        if (password !== confirmPassword) {
            alert("Passwords do not match.");
            return false; // ยกเลิกการส่งฟอร์ม
        }

        // ถ้าผ่านการตรวจสอบให้ทำการส่งข้อมูล
        registerUser();
        return false; // ไม่ให้ฟอร์มส่งข้อมูลโดยตรง
    }

    function registerUser() {
            const username = document.getElementById("regUsername").value;
            const password = document.getElementById("regPassword").value;

            // ส่งรหัสผ่านที่เป็นข้อความธรรมดาไปยังสคริปต์ PHP
            fetch('http://' + serverIP + '/registeruserstore.php', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                },
                body: JSON.stringify({ username, password }) // ส่งรหัสผ่านแบบธรรมดา
            })
            .then(response => response.json())
            .then(result => {
                if (result.success) {
                    alert(result.message);
                    document.getElementById("registerForm").reset();
                    closeRegisterModal();
                } else {
                    alert(result.message);
                }
            })
            .catch(error => {
                console.error('เกิดข้อผิดพลาดระหว่างการลงทะเบียน:', error);
                alert('เกิดข้อผิดพลาดระหว่างการลงทะเบียน.');
            });
        }
    function refund() {
    // Get data from the screen
    var cardID = document.getElementById('CardID').value; // Card number
    var amount = parseFloat(document.getElementById('balance-display').innerText); // Displayed balance

    // Validate inputs
    console.log("Card ID:", cardID);
    console.log("Amount:", amount);
    
    if (!cardID) {
        alert("Please insert your card."); // Alert if cardID is empty
        return; // Exit the function if cardID is invalid
    }

    if (isNaN(amount)) {
        console.error('Invalid amount');
        return; // Exit the function if amount is invalid
    }

    // Check if the card exists
    fetch('http://' + serverIP + '/checkCard.php', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json', // Use JSON
        },
        body: JSON.stringify({ cardID: cardID }) // Send card ID to the PHP script
    })
    .then(response => response.json())
    .then(result => {
        if (!result.success) {
            alert(result.message); // Show error if the card doesn't exist
            return; // Exit the function
        }

        // Ask user for confirmation to refund
        var confirmRefund = confirm("Are you sure you want to refund " + amount + " from card ID " + cardID + "?");

        if (!confirmRefund) {
            console.log('Refund canceled.');
            return; // Exit the function
        }

        // Create object for the data to send
        var data = {
            cardID: cardID,
            amount: amount
        };

        // Send POST request to refundCard.php
        fetch('http://' + serverIP + '/Chargestore.php', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json'
            },
            body: JSON.stringify(data) // แก้ไขให้เป็นการส่งข้อมูลจาก object `data`
        })
        .then(response => response.json()) // แปลงผลลัพธ์ให้เป็น JSON
        .then(result => {
            console.log(result); // แสดงผลลัพธ์ทั้งหมดในคอนโซล

            if (result.success) {
                // ถ้าการทำธุรกรรมสำเร็จ แสดงยอดที่บันทึก และยอดเงินคงเหลือใหม่ในบัตร
                alert(`Refund successful! \nAmount Charge: ${result.deductedAmount} \nNew balance in Card: ${result.newBalance}`);
            } else {
                // ถ้าการทำธุรกรรมล้มเหลว แสดงข้อผิดพลาด
                alert('Error: ' + result.message);
            }
        })
        .catch(error => {
            console.error('Error:', error);
        });
    })
    .catch(error => {
        console.error('Error:', error);
    });
}

    document.addEventListener('DOMContentLoaded', () => {
        const balanceDisplay = document.getElementById('balance-display');
        const buttons = document.querySelectorAll('.btn, .btn-remove, .btn-clear');

        // ตั้งค่าภาพเริ่มต้น
        balanceDisplay.textContent = '0';

        buttons.forEach(button => {
            button.addEventListener('click', () => {
                const value = button.getAttribute('data-value');

                if (value === 'C') {
                    // Clear the value of the display field
                    balanceDisplay.textContent = '0';
                    return;
                }
                if (value === 'R') {
                    // Remove the last character from the display field
                    if (balanceDisplay.textContent.length > 1) {
                        balanceDisplay.textContent = balanceDisplay.textContent.slice(0, -1);
                    } else {
                        balanceDisplay.textContent = '0'; // Set to '0.00' if nothing left
                    }
                    return;
                }

                if (value === '.') {
                    // ไม่ทำอะไรกับปุ่มจุดทศนิยม
                    return;
                } else {
                    // ตรวจสอบและตั้งค่าค่าใหม่
                    if (balanceDisplay.textContent === '0') {
                        balanceDisplay.textContent = value;
                    } else {
                        balanceDisplay.textContent += value;
                    }
                }

                // ตรวจสอบการจำกัดวงเงิน
                let inputValue = parseInt(balanceDisplay.textContent, 10);
                if (isNaN(inputValue)) {
                    inputValue = 0;
                }

                // ตรวจสอบให้ไม่เกิน 1000 บาท
                if (inputValue > 1000) {
                    balanceDisplay.textContent = '1000';
                    alert('วงเงินไม่สามารถเกิน 1000 บาท');
                }

                // จำกัดให้ใส่เลขจำนวนไม่เกิน 4 หลัก
                if (balanceDisplay.textContent.length > 4) {
                    balanceDisplay.textContent = balanceDisplay.textContent.slice(0, 4);
                }
            });
        });
    });
</script>
</body>
</html>
)rawliteral";

String getIPAddressHTML() {
  IPAddress ip = WiFi.localIP();
  return String(ip[0]) + "." + String(ip[1]) + "." + String(ip[2]) + "." + String(ip[3]);
}

#define SS_PIN  5
#define RST_PIN 27 
#define BUZZER_PIN 25

LiquidCrystal_I2C lcd(0x27, 16, 2); // กำหนด I2C LCD
MFRC522 rfid(SS_PIN, RST_PIN);      // กำหนด RFID

WebServer server(80);
WiFiManager wifiManager;

// ตัวแปรที่ใช้เก็บค่า SSID, Password และ Server IP
String ssid = "";
String password = "";
String phpServerIP = "";

// ตัวแปรสำหรับจัดการ RFID
String scannedCardID = "";

// ฟังก์ชันสำหรับบันทึกค่า WiFi และ Server IP ลง SPIFFS
void saveConfig() {
  File configFile = SPIFFS.open("/config.txt", FILE_WRITE);
  if (configFile) {
    configFile.println(ssid);
    configFile.println(password);
    configFile.println(phpServerIP);
    configFile.close();
    Serial.println("Configuration saved.");
  } else {
    Serial.println("Failed to open config file for writing.");
  }
}
// ฟังก์ชันสำหรับโหลดค่าจาก SPIFFS
void loadConfig() {
  if (SPIFFS.exists("/config.txt")) {
    File configFile = SPIFFS.open("/config.txt", FILE_READ);
    if (configFile) {
      ssid = configFile.readStringUntil('\n');
      password = configFile.readStringUntil('\n');
      phpServerIP = configFile.readStringUntil('\n');
      ssid.trim();
      password.trim();
      phpServerIP.trim();
      configFile.close();
      Serial.println("Configuration loaded:");
      Serial.println("SSID: " + ssid);
      Serial.println("Password: " + password);
      Serial.println("Server IP: " + phpServerIP);
    } else {
      Serial.println("Failed to open config file.");
    }
  } else {
    Serial.println("Config file not found.");
  }
}

void sendCardIDToServer(const String& cardID) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String serverPath = "http://" + phpServerIP + "/addCardToDatabase.php";
    http.begin(serverPath);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    String postData = "CardID=" + cardID;
    int httpResponseCode = http.POST(postData);

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("Response: " + response);
    } else {
      Serial.print("Error on sending POST: ");
      Serial.println(httpResponseCode);
    }
    http.end();
  } else {
    Serial.println("Error in WiFi connection");
  }
}

void saveCard() {
    String cardID = "123456789"; // เปลี่ยนจากการดึงค่าจาก DOM มาเป็นค่าคงที่สำหรับตัวอย่างนี้
    String body = "cardID=" + cardID; // แทนที่การใช้ backticks ด้วยการเชื่อมข้อความแบบ String

    Serial.println("Sending POST data: " + body); // ใช้ Serial.print สำหรับการ debug ใน Arduino

    if (cardID.length() == 0) {
        Serial.println("Please scan a card before saving.");
        return;
    }

    // จำลองการทำงานของ fetch ด้วย HTTP Client บน Arduino
    WiFiClient client;
    if (!client.connect("phpServerIP", 80)) { // แทนที่ localhost ด้วย IP ของ server
        Serial.println("Connection failed.");
        return;
    }

    // ส่งคำขอ HTTP POST
    client.println("POST /saveCard HTTP/1.1");
    client.println("Host: " + String(phpServerIP)); // ใช้ตัวแปร phpServerIP
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.print("Content-Length: ");
    client.println(body.length());
    client.println();
    client.println(body);

    // รอการตอบกลับจากเซิร์ฟเวอร์
    while (client.connected() || client.available()) {
        if (client.available()) {
            String line = client.readStringUntil('\n');
            Serial.println(line); // แสดงผลการตอบกลับจากเซิร์ฟเวอร์
        }
    }

    client.stop();
}

void setup() {
  Serial.begin(115200);
  SPI.begin();  // เริ่มใช้งาน SPI
  rfid.PCD_Init();  // เริ่มใช้งาน RFID
  pinMode(BUZZER_PIN, OUTPUT);

  // เริ่มต้นใช้งาน LCD
  lcd.begin();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("Initializing...");

  // เริ่มต้นใช้งาน SPIFFS
  if (!SPIFFS.begin(true)) {
    Serial.println("SPIFFS Mount Failed");
    return;
  }

  // โหลดค่า SSID, Password และ Server IP จาก SPIFFS
  loadConfig();

  // เริ่มต้น WiFiManager เพื่อให้ผู้ใช้ตั้งค่าใหม่
  WiFiManagerParameter custom_server_ip("server", "Server IP", phpServerIP.c_str(), 16);
  wifiManager.addParameter(&custom_server_ip);

  if (ssid == "" || password == "") {
    wifiManager.startConfigPortal("Store-WiFi-Setup");
  } else {
    WiFi.begin(ssid.c_str(), password.c_str());
  }

  // ถ้าเชื่อมต่อ WiFi ไม่สำเร็จ ให้เปิด WiFiManager Portal
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Failed to connect, starting configuration portal...");
    wifiManager.startConfigPortal("Store-WiFi-Setup");
  }

  // อัปเดตค่าจาก WiFiManager
  ssid = WiFi.SSID();
  password = WiFi.psk();
  phpServerIP = custom_server_ip.getValue();

  // บันทึกการตั้งค่าใหม่ลง SPIFFS
  saveConfig();

  // แสดง IP Address บน Serial Monitor และ LCD
  Serial.println("Connected to WiFi.");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("IP " + WiFi.localIP().toString());

  // เริ่มต้น Web Server
    server.on("/", HTTP_GET, []() {
        String ipAddressHTML = getIPAddressHTML();
        String htmlResponse = htmlContent;
        htmlResponse.replace("%IP_ADDRESS%", ipAddressHTML); // Insert IP address
        htmlResponse.replace("%SERVERIP%", phpServerIP); // Insert IP address
        server.send(200, "text/html", htmlResponse);
    });

      // Serve the HTML content for page2 when accessed directly
    server.on("/page2", HTTP_GET, []() {
        String isLoggedIn = server.arg("isLoggedIn");

        if (isLoggedIn == "true") {
            String page2Response = httpResponseCode;  // ใช้ตัวแปร `page2Response` แทน `response`
            page2Response.replace("%SERVERIP%", phpServerIP); // แทนที่ `%SERVERIP%` ด้วย IP ของ PHP server
            server.send(200, "text/html", page2Response);
        } else {
            server.send(403, "text/plain", "Access denied. Please log in first."); 
        }
    });
    
    server.on("/dashbord", HTTP_GET, []() {
        String dashbordResponse = datatable; // ใช้ตัวแปร `dashbordResponse` แทน `response`
        dashbordResponse.replace("%SERVERIP%", phpServerIP); // แทนที่ `%SERVERIP%` ด้วย IP ของ PHP server
        server.send(200, "text/html", dashbordResponse);
    });

    server.on("/sendCardID", HTTP_GET, []() {
        if (scannedCardID.length() > 0) {
            server.send(200, "text/plain", scannedCardID);
        } else {
            server.send(200, "text/plain", "No card detected");
        }
    });

    server.on("/saveCard", HTTP_POST, []() {
        if (server.hasArg("cardID")) {
            String cardID = server.arg("cardID");
            Serial.println("Received Card ID: " + cardID);
            sendCardIDToServer(cardID); 
            server.send(200, "text/plain", "Card saved successfully");
        } else {
            server.send(400, "text/plain", "Missing cardID parameter");
        }
    });

    server.begin();
    Serial.println("HTTP server started");
    Serial.println(getIPAddressHTML());
}
void loop() {
      server.handleClient();
 if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
        scannedCardID = "";
        for (byte i = 0; i < rfid.uid.size; i++) {
            scannedCardID += String(rfid.uid.uidByte[i] < 0x10 ? "0" : "");
            scannedCardID += String(rfid.uid.uidByte[i], HEX);
        }
        scannedCardID.toUpperCase();
        Serial.println("Card ID: " + scannedCardID);
        
                // ส่งเสียง Buzzer
        // ส่งเสียง Buzzer
        tone(BUZZER_PIN, 1000); // สร้างเสียงที่ความถี่ 1000 Hz
        delay(300);            // เล่นเสียงเป็นเวลา 1 วินาที
        noTone(BUZZER_PIN);     // หยุดเสียง
        // แสดงผล IP Address และ Card ID บน LCD

        // Call the function to update the card ID in the front-end
        server.send(200, "text/plain", scannedCardID); // Send the scanned card ID back to the client

        rfid.PICC_HaltA();
        rfid.PCD_StopCrypto1();
        lcd.setCursor(0, 1); // แสดงในบรรทัดที่สอง
        lcd.print("Card ID " + scannedCardID);

    }
    delay(100);
}

