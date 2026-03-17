<?php
include "../config/db.php";
$fid = $_GET['fid'];
$d = date("Y-m-d");
$t = date("H:i:s");

$conn->query("INSERT IGNORE INTO attendance (finger_id,date)
VALUES ($fid,'$d')");
?>

<h3>Attendance</h3>

<a href="../api/attendance_mark.php?f=morning_in&fid=<?=$fid?>">Morning IN</a><br>
<a href="../api/attendance_mark.php?f=noon_out&fid=<?=$fid?>">Noon OUT</a><br>
<a href="../api/attendance_mark.php?f=noon_in&fid=<?=$fid?>">Noon IN</a><br>
<a href="../api/attendance_mark.php?f=evening_out&fid=<?=$fid?>">Evening OUT</a>
