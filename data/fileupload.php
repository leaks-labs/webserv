<?php
if ($_SERVER['REQUEST_METHOD'] != 'POST'){
    echo 'method is not post';
    exit;
}
if (empty($_SERVER['PATH_INFO'])){
    echo 'upload unavalaible: path_info is not defined in the server config';
    exit;
}
if (!isset($_FILES['file']) || $_FILES['file']['error'] != UPLOAD_ERR_OK) {
    echo 'erreur lors de l\'upload du fichier';
    exit;
}

$fileTmpPath = $_FILES['file']['tmp_name'];
$fileName = $_FILES['file']['name'];
$fileSize = $_FILES['file']['size'];
$fileType = $_FILES['file']['type'];
$fileNameCmps = explode(".", $fileName);
$fileExtension = strtolower(end($fileNameCmps));

$uploadFileDir = $_SERVER['PATH_INFO'];
$dest_path = $uploadFileDir . $fileName;

if (!is_dir($uploadFileDir)) {
    mkdir($uploadFileDir, 0777, true);
}
if(move_uploaded_file($fileTmpPath, $dest_path))
    echo 'Le fichier a été téléchargé avec succès.';
else
    echo 'Erreur lors du téléchargement du fichier.';
?>
