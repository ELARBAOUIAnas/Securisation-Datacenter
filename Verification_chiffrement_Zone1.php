<?php
ini_set('display_errors', 1);
error_reporting(E_ALL);

// Vérifier que le paramètre POST 'uid' est présent
if (!isset($_POST['uid'])) {
    die("UID manquant");
}

$encryptedHexUID = $_POST['uid'];
// Pour le code, il peut être fourni ou non
$encryptedHexCode = isset($_POST['code']) ? $_POST['code'] : null;

// Convertir l'UID chiffré de HEX en binaire
$encryptedUID = hex2bin($encryptedHexUID);
if ($encryptedUID === false) {
    die("Erreur de conversion HEX en binaire pour l'UID");
}

// Pour le code, s'il est fourni, conversion de HEX en binaire
if ($encryptedHexCode) {
    $encryptedCode = hex2bin($encryptedHexCode);
    if ($encryptedCode === false) {
        die("Erreur de conversion HEX en binaire pour le code");
    }
} else {
    $encryptedCode = null;
}

// Clé AES et IV (doivent correspondre à ceux utilisés sur l'Arduino)
$keyHex = "bc026d8654721c298865b188318b8a28"; // Clé AES 128 bits (16 octets)
$ivHex  = "000102030405060708090A0B0C0D0E0F"; // IV de 16 octets

$key = hex2bin($keyHex);
$iv  = hex2bin($ivHex);
if ($key === false || $iv === false) {
    die("Erreur de conversion de la clé ou de l'IV");
}

// Fonction de déchiffrement AES-128-CBC avec suppression du padding PKCS7
function decryptAES($encryptedData, $key, $iv) {
    $decrypted = openssl_decrypt($encryptedData, "AES-128-CBC", $key, OPENSSL_RAW_DATA, $iv);
    if ($decrypted === false) {
        return null;
    }
    // Suppression du padding PKCS7
    $pad = ord($decrypted[strlen($decrypted) - 1]);
    if ($pad > 0 && $pad <= 16) {
        $decrypted = substr($decrypted, 0, -$pad);
    }
    return $decrypted;
}

// Déchiffrer l'UID
$decryptedUID = decryptAES($encryptedUID, $key, $iv);
if ($decryptedUID === null || empty($decryptedUID)) {
    die("Erreur: UID non déchiffré !");
}
$uid = rtrim($decryptedUID, "\0");

// Si un code est envoyé, le déchiffrer
if ($encryptedCode) {
    $decryptedCode = decryptAES($encryptedCode, $key, $iv);
    if ($decryptedCode === null || empty($decryptedCode)) {
        die("Erreur: Code non déchiffré !");
    }
    $code = rtrim($decryptedCode, "\0");
} else {
    $code = null;
}

// Connexion à la base de données
$conn = new mysqli('localhost', 'anous', 'Anousa@2006', 'Datacenter_v1');
if ($conn->connect_error) {
    die("Erreur connexion DB: " . $conn->connect_error);
}

$date_heure = date('Y-m-d H:i:s');

if ($code === null) {
    // Étape 1 : Seul l'UID est fourni (Zone 0)
    $sql = "SELECT e.nom, e.prenom, r.bloque
            FROM RFID_Zone1 r
            JOIN employes_zone1 e ON r.employe_id = e.id
            WHERE r.UID = ?";
    $stmt = $conn->prepare($sql);
    $stmt->bind_param('s', $uid);
    $stmt->execute();
    $result = $stmt->get_result();
  
    if ($result->num_rows > 0) {
        $row = $result->fetch_assoc();
        if ($row['bloque'] == 1) {
            echo "NO";
        } else {
            echo "OK";
        }
    } else {
        echo "NO";
        // Insertion d'une alerte pour UID inconnu (employe_id = NULL)
        $sql_insert_alert = "INSERT INTO alerte_zone1 (date_alerte, employe_id, description) VALUES (?, NULL, ?)";
        $stmt_alert = $conn->prepare($sql_insert_alert);
        $description = "Tentative d'accès avec UID inconnu : $uid";
        $stmt_alert->bind_param('ss', $date_heure, $description);
        $stmt_alert->execute();
        $stmt_alert->close();
    }
    $stmt->close();
    $conn->close();
    exit();
} else {
    // Étape 2 : L'UID et le code sont fournis (Zone 1)
    $sql = "SELECT e.nom, e.prenom, r.bloque, r.tentatives_echouees, d.code, r.id AS rfid_id, e.id AS employe_id
            FROM RFID_Zone1 r
            JOIN employes_zone1 e ON r.employe_id = e.id
            JOIN Digicode_Zone1 d ON e.id = d.employe_id
            WHERE r.UID = ?";
    $stmt = $conn->prepare($sql);
    $stmt->bind_param('s', $uid);
    $stmt->execute();
    $result = $stmt->get_result();
  
    if ($result->num_rows > 0) {
        $row = $result->fetch_assoc();
        if ($row['bloque'] == 1) {
            echo "NO";
            $stmt->close();
            $conn->close();
            exit();
        }
        $nom = $row['nom'];
        $prenom = $row['prenom'];
        $tentatives = $row['tentatives_echouees'];
        $code_stocke = $row['code'];
        $rfid_id = $row['rfid_id'];
        $employe_id = $row['employe_id'];
       
        if ($code === $code_stocke) {
            echo "OK";
            // Réinitialiser le nombre de tentatives échouées
            $sql_reset = "UPDATE RFID_Zone1 SET tentatives_echouees = 0 WHERE id = ?";
            $stmt_reset = $conn->prepare($sql_reset);
            $stmt_reset->bind_param('i', $rfid_id);
            $stmt_reset->execute();
            $stmt_reset->close();
           
            // Enregistrer l'accès dans acces_zone1
            $description = "Accès autorisé pour $nom $prenom";
            $sql_insert_acces = "INSERT INTO acces_zone1 (date_acces, employe_id, description) VALUES (?, ?, ?)";
            $stmt_insert_acces = $conn->prepare($sql_insert_acces);
            $stmt_insert_acces->bind_param('sis', $date_heure, $employe_id, $description);
            $stmt_insert_acces->execute();
            $stmt_insert_acces->close();
        } else {
            // Code incorrect, incrémenter le nombre de tentatives
            $tentatives++;
            $sql_update = "UPDATE RFID_Zone1 SET tentatives_echouees = ? WHERE id = ?";
            $stmt_update = $conn->prepare($sql_update);
            $stmt_update->bind_param('ii', $tentatives, $rfid_id);
            $stmt_update->execute();
            $stmt_update->close();
           
            if ($tentatives >= 3) {
                // Bloquer l'utilisateur après 3 tentatives
                $sql_block = "UPDATE RFID_Zone1 SET bloque = 1 WHERE id = ?";
                $stmt_block = $conn->prepare($sql_block);
                $stmt_block->bind_param('i', $rfid_id);
                $stmt_block->execute();
                $stmt_block->close();
               
                echo "NO";
                $description = "Accès bloqué pour $nom $prenom après 3 tentatives incorrectes";
            } else {
                echo "NO";
                $description = "Mauvais code saisi par $nom $prenom (Tentative $tentatives/3)";
            }
            // Enregistrer l'alerte
            $sql_insert_alert = "INSERT INTO alerte_zone1 (date_alerte, employe_id, description) VALUES (?, ?, ?)";
            $stmt_insert_alert = $conn->prepare($sql_insert_alert);
            $stmt_insert_alert->bind_param('sis', $date_heure, $employe_id, $description);
            $stmt_insert_alert->execute();
            $stmt_insert_alert->close();
        }
    } else {
        echo "NO";
        // UID invalide → Enregistrer une alerte pour UID inconnu
        $sql_insert_alert = "INSERT INTO alerte_zone1 (date_alerte, employe_id, description) VALUES (?, NULL, ?)";
        $stmt_insert_alert = $conn->prepare($sql_insert_alert);
        $description = "Tentative d'accès avec UID inconnu : $uid";
        $stmt_insert_alert->bind_param('ss', $date_heure, $description);
        $stmt_insert_alert->execute();
        $stmt_insert_alert->close();
    }
    $stmt->close();
    $conn->close();
    exit();
}
?>
 


