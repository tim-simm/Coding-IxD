<?php
$servername = "usersql.zedat.fu-berlin.de";
$username = "lamit03-sql";
$password = "4hwy6eq5";
$dbname = "lamit03-db1";

$conn = new mysqli($servername, $username, $password, $dbname);

// Überprüfen Sie die Verbindung
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
}

// Daten aus dem POST-Request abrufen
$age = $_POST['age'];
$height = $_POST['height'];
$result =$_POST['result'];

// SQL-Abfrage zum Einfügen der Daten in die Tabelle
$sql = "INSERT INTO BSIQ_tabelle (result, age, height) VALUES ('$result', '$age', '$height')";

if ($conn->query($sql) === TRUE) {
    echo "Data saved successfully";
} else {
    echo "Error saving to database: " . $conn->error;
}

$conn->close();
?>