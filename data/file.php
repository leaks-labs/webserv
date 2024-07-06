<?php
$arg1 = isset($_GET['arg1']) ? htmlspecialchars($_GET['arg1']) : '';
echo "<!DOCTYPE html>\n";
echo "<html>\n<body>\n";
echo "<h1>PHP PAGE : $arg1</h1>\n";
echo "</body>\n</html>";
?>
