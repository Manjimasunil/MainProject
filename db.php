<?php
$conn = new mysqli("localhost", "root", "", "fingerprint_db");
date_default_timezone_set("Asia/Kolkata");
if ($conn->connect_error) die("DB Error");
?>
