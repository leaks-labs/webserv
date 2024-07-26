<?php
$title = isset($_POST['title']) ? htmlspecialchars($_POST['title']) : '';
$content = isset($_POST['content']) ? htmlspecialchars($_POST['content']) : '';
$file_content = '';
$file_type = '';

if (isset($_FILES['file']) && $_FILES['file']['error'] == UPLOAD_ERR_OK) {
    $file_type = $_FILES['file']['type'];
    $file_content = file_get_contents($_FILES['file']['tmp_name']);
}

echo "<!DOCTYPE html>\n";
echo "<html>\n<body>\n";
echo "<h1>$title</h1>\n";
echo "<h2>$content</h2>\n";

if (!empty($file_content)) {
    if (strpos($file_type, 'text') !== false) {
        echo "<pre>\n";
        echo htmlspecialchars($file_content);
        echo "</pre>\n";
    } elseif (strpos($file_type, 'image') !== false) {
        echo "<img src='data:$file_type;base64," . base64_encode($file_content) . "' alt='Uploaded image'>\n";
    } else {
        echo "<p>Le type de fichier n'est pas pris en charge pour l'affichage direct.</p>\n";
    }
} else {
    echo "<p>Aucun fichier téléchargé ou fichier invalide.</p>\n";
}

#echo "<pre>\n";
#print_r($_GET);
#print_r($_POST);
#print_r($_FILES);
#echo "</pre>\n";

echo "</body>\n</html>";
?>
