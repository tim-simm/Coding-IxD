<?php
$servername = "usersql.zedat.fu-berlin.de";
$username = "lamit03-sql";
$password = "4hwy6eq5";
$dbname = "lamit03-db1";

$conn = new mysqli($servername, $username, $password, $dbname);

if ($conn->connect_error) {
    die("Verbindung zur MySQL-Datenbank fehlgeschlagen: " . $conn->connect_error);
}

$query = "SELECT  result,height,age FROM BSIQ_tabelle ORDER BY id DESC LIMIT 1";
$result = $conn->query($query);

if ($result->num_rows > 0) {
    // Neuesten Score auslesen und als JSON-Antwort senden
    $row = $result->fetch_assoc();
    $response = array((int)$row['result'], (int)$row['height'], (int)$row['age']);
    echo json_encode($response);
} else {
    // Keine Daten gefunden
    echo "Keine Daten gefunden.";
}

$conn->close();
?>