<?php
ini_set('display_errors', 1);
error_reporting(E_ALL);

if (!isset($_POST['id'])) {
    die("Empreinte ID manquant");
}

$empreinte_id = $_POST['id'];
$encryptedHexCode = isset($_POST['code']) ? $_POST['code'] : null;

// Clé AES et IV pour le chiffrement
$keyHex = "bc026d8654721c298865b188318b8a28";
$ivHex  = "000102030405060708090A0B0C0D0E0F";

$key = hex2bin($keyHex);
$iv  = hex2bin($ivHex);
if ($key === false || $iv === false) {
    die("Erreur de conversion de la clé ou de l'IV");
}

// Fonction de déchiffrement
function decryptAES($encryptedData, $key, $iv) {
    $decrypted = openssl_decrypt($encryptedData, "AES-128-CBC", $key, OPENSSL_RAW_DATA, $iv);
    if ($decrypted === false) {
        return null;
    }
    $pad = ord($decrypted[strlen($decrypted) - 1]);
    if ($pad > 0 && $pad <= 16) {
        $decrypted = substr($decrypted, 0, -$pad);
    }
    return $decrypted;
}

// Déchiffrement du code s'il est fourni
if ($encryptedHexCode) {
    $encryptedCode = hex2bin($encryptedHexCode);
    if ($encryptedCode === false) {
        die("Erreur de conversion HEX en binaire pour le code");
    }
    
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

// Vérifier si l'utilisateur existe
$sql = "SELECT ez.id, ez.employe_id, e.nom, e.prenom, ez.tentatives_echouees, ez.bloque, d.code
        FROM empreinte_Zone3 ez
        LEFT JOIN employes_zone3 e ON ez.employe_id = e.id
        LEFT JOIN Digicode_Zone3 d ON ez.employe_id = d.employe_id
        WHERE ez.empreinte_id = ?";
$stmt = $conn->prepare($sql);
$stmt->bind_param('i', $empreinte_id);
$stmt->execute();
$result = $stmt->get_result();

if ($result->num_rows > 0) {
    $row = $result->fetch_assoc();
    
    // Récupération des données
    $empreinte_db_id = $row['id'];
    $employe_id = $row['employe_id'];
    $nom = $row['nom'];
    $prenom = $row['prenom'];
    $tentatives = $row['tentatives_echouees'];
    $code_stocke = $row['code'];
    $bloque = $row['bloque'];

    // Vérification du blocage
    if ($bloque == 1) {
        echo "BQ";
    } else {
        if ($code === null) {
            // Cas où seul l'empreinte est fournie
            echo "OK";
        } else {
            // Vérification du code
            if ($code === $code_stocke) {
                echo "OK";

                // Réinitialisation des tentatives échouées
                $sql_reset = "UPDATE empreinte_Zone3 SET tentatives_echouees = 0 WHERE id = ?";
                $stmt_reset = $conn->prepare($sql_reset);
                $stmt_reset->bind_param('i', $empreinte_db_id);
                $stmt_reset->execute();
                $stmt_reset->close();

                // Enregistrement de l'accès
                $description = "Accès autorisé pour $nom $prenom";
                $sql_insert_acces = "INSERT INTO acces_zone3 (date_acces, employe_id, description) VALUES (?, ?, ?)";
                $stmt_insert_acces = $conn->prepare($sql_insert_acces);
                $stmt_insert_acces->bind_param('sis', $date_heure, $employe_id, $description);
                $stmt_insert_acces->execute();
                $stmt_insert_acces->close();
            } else {
                // Incrémentation des tentatives échouées
                $tentatives++;
                $sql_update = "UPDATE empreinte_Zone3 SET tentatives_echouees = ? WHERE id = ?";
                $stmt_update = $conn->prepare($sql_update);
                $stmt_update->bind_param('ii', $tentatives, $empreinte_db_id);
                $stmt_update->execute();
                $stmt_update->close();

                // Bloquer l'utilisateur après 3 tentatives incorrectes
                if ($tentatives >= 3) {
                    $sql_block = "UPDATE empreinte_Zone3 SET bloque = 1 WHERE id = ?";
                    $stmt_block = $conn->prepare($sql_block);
                    $stmt_block->bind_param('i', $empreinte_db_id);
                    $stmt_block->execute();
                    $stmt_block->close();

                    echo "NO";
                    $description = "Accès bloqué pour $nom $prenom après 3 tentatives incorrectes";
                } else {
                    echo "NO";
                    $description = "Mauvais code saisi par $nom $prenom (Tentative $tentatives/3)";
                }

                // Enregistrement de l'alerte
                $sql_insert_alert = "INSERT INTO alerte_zone3 (date_alerte, employe_id, description) VALUES (?, ?, ?)";
                $stmt_insert_alert = $conn->prepare($sql_insert_alert);
                $stmt_insert_alert->bind_param('sis', $date_heure, $employe_id, $description);
                $stmt_insert_alert->execute();
                $stmt_insert_alert->close();
            }
        }
    }
} else {
    // Empreinte inconnue → Enregistrer une alerte
    echo "NO";
    $sql_insert_alert = "INSERT INTO alerte_zone3 (date_alerte, employe_id, description) VALUES (?, NULL, ?)";
    $stmt_insert_alert = $conn->prepare($sql_insert_alert);
    $description = "Tentative d'accès avec empreinte ID inconnu : $empreinte_id";
    $stmt_insert_alert->bind_param('ss', $date_heure, $description);
    $stmt_insert_alert->execute();
    $stmt_insert_alert->close();
}

$stmt->close();
$conn->close();
exit();
?>
