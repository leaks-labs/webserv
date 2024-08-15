<?php
if ($_SERVER["REQUEST_METHOD"] == "POST") {
    $title = htmlspecialchars($_POST["title"]);
    $content = htmlspecialchars($_POST["content"]);

    echo "<h1>$title</h1>";
    echo "<p>$content</p>";
    
    if (isset($_FILES["file"]) && $_FILES["file"]["error"] == UPLOAD_ERR_OK) {
        $file = $_FILES["file"];

        $target_directory = "uploads/";
        $file_path = $target_directory . basename($file["name"]);
        $upload_ok = move_uploaded_file($file["tmp_name"], $file_path);

        if ($upload_ok) {
            echo "<p>Fichier téléchargé : " . htmlspecialchars($file["name"]) . "</p>";
            echo "<p>Type de fichier : " . $file["type"] . "</p>";
            
            $file_type = strtolower(pathinfo($file_path, PATHINFO_EXTENSION));
            if (in_array($file_type, ["jpg", "jpeg", "png", "gif"])) {
                echo "<img src='$file_path' alt='Image téléchargée'>";
            } elseif ($file_type == "txt") {
                echo "<pre>" . htmlspecialchars(file_get_contents($file_path)) . "</pre>";
            } else {
                echo "<p>Le fichier a été téléchargé avec succès. <a href='$file_path'>Cliquez ici pour le télécharger</a>.</p>";
            }
        } else {
            echo "<p>Erreur lors du téléchargement du fichier.</p>";
        }
    }

} else {
    header("Location: postAll.html");
    exit();
}
?>
