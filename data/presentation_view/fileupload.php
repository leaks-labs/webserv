<?php
if ($_SERVER['REQUEST_METHOD'] != 'POST'){
    echo 'method is not post';
    exit;
}
if (!isset($_SERVER['PATH_INFO'])){
    echo 'upload unavalaible: path_info is not defined in the server config';
    exit;
}
if (!isset($_FILES['file'])) {
    echo 'erreur lors de l\'upload du fichier';
    exit;
}


if ($_FILES['file']['error'] != UPLOAD_ERR_OK) {
   switch ($_FILES['file']['error']) {
       case UPLOAD_ERR_INI_SIZE:
           echo 'The uploaded file exceeds the upload_max_filesize directive in php.ini.';
           break;
       case UPLOAD_ERR_FORM_SIZE:
           echo 'The uploaded file exceeds the MAX_FILE_SIZE directive that was specified in the HTML form.';
           break;
       case UPLOAD_ERR_PARTIAL:
           echo 'The uploaded file was only partially uploaded.';
           break;
       case UPLOAD_ERR_NO_FILE:
           echo 'No file was uploaded.';
           break;
       case UPLOAD_ERR_NO_TMP_DIR:
           echo 'Missing a temporary folder.';
           break;
       case UPLOAD_ERR_CANT_WRITE:
           echo 'Failed to write file to disk.';
           break;
       case UPLOAD_ERR_EXTENSION:
           echo 'A PHP extension stopped the file upload.';
           break;
       default:
           echo 'Unknown error occurred during file upload.';
           break;
   }
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
