<?php
// Verbindung zur Datenbank herstellen (ersetze die Platzhalter durch deine echten Daten)
$servername = "usersql.zedat.fu-berlin.de";
$username = "lamit03-sql";
$password = "4hwy6eq5";
$dbname = "lamit03-db1";

$conn = new mysqli($servername, $username, $password, $dbname);

// Überprüfen, ob die Verbindung erfolgreich war
if ($conn->connect_error) {
    die("Verbindung fehlgeschlagen: " . $conn->connect_error);
}

// Ergebnisse der Fragen aus dem Formular lesen
//$antwort1 = isset($_POST['frage1']) ? intval($_POST['frage1']) : 0;
//$antwort2 = isset($_POST['frage2']) ? intval($_POST['frage2']) : 0;

$answer1 = intval($_REQUEST['Answer1']);
$answer2 = intval($_REQUEST['Answer2']);
$answer3 = intval($_REQUEST['Answer3']);
$answer4 = intval($_REQUEST['Answer4']);
$answer5 = intval($_REQUEST['Answer5']);
$answer6 = intval($_REQUEST['Answer6']);
$answer7 = intval($_REQUEST['Answer7']);
$answer8 = intval($_REQUEST['Answer8']);
$answer9 = intval($_REQUEST['Answer9']);
$answer10 = intval($_REQUEST['Answer10']);

// Gesamtergebnis berechnen
$score = $answer1 + $answer2 + $answer3 + $answer4 + $answer5 + $answer6 + $answer7 + $answer8 + $answer9 + $answer10;

$age = intval($_POST['age']);
$height = intval($_POST['height']);
$gender = intval($_POST['gender']);

// Das Ergebnis in die Datenbank speichern (ersetze dies durch deine echte Logik)
$sql = "INSERT INTO BSIQ_tabelle (score, height, age, gender) VALUES ('$score', '$height', '$age', '$gender')";

if ($conn->query($sql) === TRUE) {
    echo "Umfrageergebnis erfolgreich in der Datenbank gespeichert.";
} else {
    echo "Fehler beim Speichern des Umfrageergebnisses: " . $conn->error;
}

// Verbindung zur Datenbank schließen
$conn->close();

header("Location: https://lamit03.userpage.fu-berlin.de/Final/");
exit();
?>
