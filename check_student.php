<?php
include("../config/db.php");

// ------------------ DB CHECK ------------------
if ($conn->connect_error) {
    die("DB connection failed: " . $conn->connect_error);
}

// ------------------ GET FID ------------------
$fid = $_GET['fid'] ?? 0;
$fid = (int)$fid;

// ------------------ PREPARED QUERY ------------------
$sql = "SELECT * FROM students WHERE fid = ?";
$stmt = $conn->prepare($sql);

// ------------------ CHECK PREPARE ------------------
if (!$stmt) {
    die("Prepare failed: " . $conn->error);
}

$stmt->bind_param("i", $fid);
$stmt->execute();

$result = $stmt->get_result();

// ------------------ RESULT CHECK ------------------
if ($result && $result->num_rows > 0) {
    $row = $result->fetch_assoc();

    // OPTIONAL: update attendance status here
    // $conn->query("UPDATE students SET status='present' WHERE fid=$fid");

    echo "REGISTERED";
} else {
    echo "NOT REGISTERED";
}

$stmt->close();
$conn->close();
?>

}
