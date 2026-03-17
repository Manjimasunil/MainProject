<?php
include "../config/db.php";

$total = intval($_POST['total_fee']);
$paid  = intval($_POST['paid_fee']);
$bal   = $total - $paid;

$stmt = $conn->prepare(
  "UPDATE students SET
    register_no=?,
    name=?,
    department=?,
    semester=?,
    contact=?,
    father_name=?,
    father_contact=?,
    mother_name=?,
    mother_contact=?,
    total_fee=?,
    paid_fee=?,
    balance_fee=?,
    status='active'
   WHERE finger_id=?"
);

$stmt->bind_param(
  "sssssssssiiii",
  $_POST['register_no'],
  $_POST['name'],
  $_POST['department'],
  $_POST['semester'],
  $_POST['contact'],
  $_POST['father_name'],
  $_POST['father_contact'],
  $_POST['mother_name'],
  $_POST['mother_contact'],
  $total,
  $paid,
  $bal,
  $_POST['finger_id']
);

$stmt->execute();

// ✅ Redirect after successful save
header("Location: ../pages/dashboard.php");
