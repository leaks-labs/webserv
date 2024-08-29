<?php
if ($_SERVER["REQUEST_METHOD"] == "POST") {
    $title = htmlspecialchars($_POST["title"]);
    $content = htmlspecialchars($_POST["content"]);

    echo "<h1>$title</h1>";
    echo "<p>$content</p>";
    
    if (isset($_FILES["file"]) && $_FILES["file"]["error"] == UPLOAD_ERR_OK) {
        $file = $_FILES["file"];
        $file_name = $file["name"];

        ////
        $target_directory = "uploads/";
        $file_path = $target_directory . basename($file_name);
        $upload_ok = move_uploaded_file($file["tmp_name"], $file_path);
        // $file_path = $file["tmp_name"];
        ////

        if ($upload_ok) {
            echo "<p>Fichier téléchargé : " . htmlspecialchars($file_name) . "</p>";
            echo "<p>Type de fichier : " . $file["type"] . "</p>";
            $file_type = strtolower(pathinfo($file_name, PATHINFO_EXTENSION));
            if (in_array($file_type, ["jpg", "jpeg", "png", "gif"])) {
                ////
                // $file_data = base64_encode(file_get_contents($file_path));
                // $src = "data:image/$file_type;base64," . $file_data;
                // echo "<img src=\"$src\" alt=\"Image téléchargée\">";
                echo "<img src='$file_path' alt='Image téléchargée'>";
                ////
            } elseif ($file_type == "txt") {
                echo "<pre>" . htmlspecialchars(file_get_contents($file_path)) . "</pre>";
            } else {
                // do not use when the tmp-file is used. Use only when the file is moved to a directory
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