<?php
$title = isset($_GET['title']) ? htmlspecialchars($_GET['title']) : '';
echo "<!DOCTYPE html>\n";
echo "<html>\n<body>\n";
echo "<h1>$title</h1>\n";
print_r($_GET);
print_r($_POST);
echo "</html>\n</body>\n";
?>
