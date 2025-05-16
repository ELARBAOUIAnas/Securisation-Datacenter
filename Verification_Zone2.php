<?php
ini_set('display_errors', 1);
error_reporting(E_ALL);

// Vérifier que le paramètre POST 'reponse' est présent
if (!isset($_POST['reponse'])) {
    die("Donnée manquante");
}

$response = $_POST['reponse'];
error_log("Valeur reçue: " . $response);

// Connexion à la base de données
$conn = new mysqli('localhost', 'anous', 'Anousa@2006', 'Datacenter_v1');
if ($conn->connect_error) {
    die("Erreur connexion DB: " . $conn->connect_error);
}

$date_heure = date('Y-m-d H:i:s');

if ($response === "NO") {
    // Empreinte non reconnue
    echo "NO";
    $description = "Tentative d'accès avec empreinte inconnue";
    $sql_alert = "INSERT INTO alerte_zone2 (date_alerte, employe_id, description) VALUES (?, NULL, ?)";
    $stmt_alert = $conn->prepare($sql_alert);
    $stmt_alert->bind_param('ss', $date_heure, $description);
    $stmt_alert->execute();
    $stmt_alert->close();
} else {
    // Convertir la réponse en entier
    $empreinte_id = (int)$response;
    error_log("Empreinte ID convertie: " . $empreinte_id);

    // Requête pour récupérer les informations de l'employé à partir de l'empreinte
    $sql = "SELECT e.nom, e.prenom, e.id AS employe_id
            FROM empreinte_Zone2 ep
            JOIN employes_zone2 e ON ep.employe_id = e.id
            WHERE ep.empreinte_id = ?";
    $stmt = $conn->prepare($sql);
    $stmt->bind_param('i', $empreinte_id);
    $stmt->execute();
    $result = $stmt->get_result();
    
    error_log("Nombre de lignes retournées: " . $result->num_rows);

    if ($result->num_rows > 0) {
        // Empreinte reconnue
        $row = $result->fetch_assoc();
        $nom = $row['nom'];
        $prenom = $row['prenom'];
        $employe_id = $row['employe_id'];
        echo "OK";

        // Enregistrer l'accès dans acces_zone2
        $description = "Accès autorisé pour $nom $prenom";
        $sql_insert_acces = "INSERT INTO acces_zone2 (date_acces, employe_id, description) VALUES (?, ?, ?)";
        $stmt_insert_acces = $conn->prepare($sql_insert_acces);
        $stmt_insert_acces->bind_param('sis', $date_acces, $employe_id, $description);
        $stmt_insert_acces->execute();
        $stmt_insert_acces->close();
    } else {
        echo "NO";
        // Empreinte non reconnue, insertion d'une alerte
        $description = "Tentative d'accès avec empreinte inconnue: " . $empreinte_id;
        $sql_alert = "INSERT INTO alerte_zone2 (date_alerte, employe_id, description) VALUES (?, NULL, ?)";
        $stmt_alert = $conn->prepare($sql_alert);
        $stmt_alert->bind_param('ss', $date_heure, $description);
        $stmt_alert->execute();
        $stmt_alert->close();
    }
    $stmt->close();
}

$conn->close();
?>
