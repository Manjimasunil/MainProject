<?php
include "../config/db.php";

$finger_id = intval($_GET['fid']);

// 🔐 AUTO-INSERT if not exists
$check = $conn->query("SELECT finger_id FROM students WHERE finger_id=$finger_id");

if ($check->num_rows == 0) {
  $conn->query(
    "INSERT INTO students (finger_id, status)
     VALUES ($finger_id, 'pending')"
  );
}
?>
<link rel="stylesheet" href="../api/style.css">

<form action="../api/enroll_save.php" method="post">
  <input type="hidden" name="finger_id" value="<?= $finger_id ?>">

  Register No: <input name="register_no" required><br>
  Name: <input name="name" required><br>

  Department:
  <select name="department">
    <option>BCA</option><option>MCA</option>
    <option>BTECH</option><option>MTECH</option>
  </select><br>

  Semester:
  <select name="semester">
    <option>1st</option><option>2nd</option><option>3rd</option>
    <option>4th</option><option>5th</option><option>6th</option>
  </select><br>

  Contact: <input name="contact"><br>

  Father Name: <input name="father_name"><br>
  Father Contact: <input name="father_contact"><br>

  Mother Name: <input name="mother_name"><br>
  Mother Contact: <input name="mother_contact"><br>

  Total Fee: <input name="total_fee"><br>
  Paid Fee: <input name="paid_fee"><br>

  <button type="submit">Save Student</button>
</form>
