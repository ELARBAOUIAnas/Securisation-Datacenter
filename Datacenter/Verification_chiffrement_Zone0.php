<?php
ini_set('display_errors', 1);
//error_reporting(E_ALL);

// Vérifier que l'UID chiffré est présent
if (!isset($_POST['uid'])) {
    die("UID chiffré manquant");
}

$encryptedHex = $_POST['uid'];

// Convertir la chaîne HEX en binaire
$encryptedData = hex2bin($encryptedHex);
if ($encryptedData === false) {
    die("Erreur de conversion HEX en binaire");
}

// Clé AES et IV (doivent correspondre exactement à ceux utilisés sur Arduino)
$keyHex = "bc026d8654721c298865b188318b8a28"; // Clé AES 128 bits
$ivHex  = "000102030405060708090A0B0C0D0E0F"; // IV de 16 octets

$key = hex2bin($keyHex);
$iv  = hex2bin($ivHex);
if ($key === false || $iv === false) {
    die("Erreur de conversion de la clé ou de l'IV");
}

// Déchiffrer la donnée avec AES-128-CBC
function decryptUID($encryptedData, $key, $iv) {
    // Vérifier si les données sont valides
    if (!$encryptedData) {
        echo "AB" ;
    }

    // Déchiffrement avec OpenSSL en AES-128-CBC
    $decrypted = openssl_decrypt($encryptedData, "AES-128-CBC", $key, OPENSSL_RAW_DATA, $iv);

    // Vérifier si le déchiffrement a réussi
    if ($decrypted === false) {
        echo "AC";
    }

    // Suppression du padding PKCS7
    $pad = ord($decrypted[strlen($decrypted) - 1]);
    if ($pad > 0 && $pad <= 16) {
        $decrypted = substr($decrypted, 0, -$pad);
    }

    return $decrypted;
}

// Déchiffrer l'UID
$decryptedUID = decryptUID($encryptedData, $key, $iv);

// Vérifier si le déchiffrement a réussi
if ($decryptedUID === false || empty($decryptedUID)) {
    die("Erreur: UID non déchiffré !");
}

// Nettoyer l'UID
$uid = rtrim($decryptedUID, "\0");

// Afficher l'UID pour vérifier (optionnel)
//file_put_contents("log.txt", "UID déchiffré : " . $uid . PHP_EOL, FILE_APPEND);

// Connexion à la base de données
$conn = new mysqli('localhost', 'anous', 'Anousa@2006', 'Datacenter_v1');
if ($conn->connect_error) {
    die("Erreur connexion DB: " . $conn->connect_error);
}

$date_heure = date('Y-m-d H:i:s');

// Vérifier si l'UID existe dans RFID_Zone0 en joignant avec employes_zone0
$sql = "SELECT e.nom, e.prenom, r.employe_id
       FROM RFID_Zone0 r
       JOIN employes_zone0 e ON r.employe_id = e.id
       WHERE r.UID = ?";
$stmt = $conn->prepare($sql);
$stmt->bind_param('s', $uid);
$stmt->execute();
$result = $stmt->get_result();

if ($result->num_rows > 0) {
    // UID valide → récupérer nom, prénom et employe_id
    $row = $result->fetch_assoc();
    $nom = $row['nom'];
    $prenom = $row['prenom'];
    $employe_id = $row['employe_id'];
    echo "OK";

    // Enregistrer l'accès dans acces_zone0
    $description = "Accès autorisé pour $nom $prenom";
    $sql_insert_acces = "INSERT INTO acces_zone0 (date_acces, employe_id, description) VALUES (?, ?, ?)";
    $stmt_insert_acces = $conn->prepare($sql_insert_acces);
    $stmt_insert_acces->bind_param('sis', $date_heure, $employe_id, $description);
    $stmt_insert_acces->execute();
    $stmt_insert_acces->close();
} else {
    echo "NO";
    // UID invalide → Enregistrer une alerte
    $description = "Tentative d'accès non autorisée avec UID: " . $uid;
    $sql_insert_alert = "INSERT INTO alerte_zone0 (date_alerte, employe_id, description) VALUES (?, NULL, ?)";
    $stmt_insert_alert = $conn->prepare($sql_insert_alert);
    $stmt_insert_alert->bind_param('ss', $date_heure, $description);
    $stmt_insert_alert->execute();
    $stmt_insert_alert->close();
}

$stmt->close();
$conn->close();
?>
