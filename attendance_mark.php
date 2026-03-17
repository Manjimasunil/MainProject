<?php
include "../config/db.php";
$fid = $_GET['fid'];
$f = $_GET['f'];
$d = date("Y-m-d");
$t = date("H:i:s");

$conn->query("UPDATE attendance SET $f='$t'
WHERE finger_id=$fid AND date='$d'");

header("Location: ../pages/dashboard.php");
