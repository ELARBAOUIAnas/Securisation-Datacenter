/*
#include "zone0.h"
#include "menu.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QTimer>
#include <QHeaderView>

Zone0::Zone0(QWidget *parent) : QWidget(parent)
{
    setWindowTitle("Zone 0");
    resize(600, 400);

    QPushButton *historiqueAlertesButton = new QPushButton("Historique des alertes");
    QPushButton *historiqueAccesButton = new QPushButton("Historique des accès");
    QPushButton *listeEmployesButton = new QPushButton("Liste des employés ayant accès");
    QPushButton *retourButton = new QPushButton("Retour");

    historiqueAlertesButton->setFixedSize(300, 40);
    historiqueAccesButton->setFixedSize(300, 40);
    listeEmployesButton->setFixedSize(300, 40);
    retourButton->setFixedSize(300, 40);

    infoLabel = new QLabel("Sélectionnez une option.");
    infoLabel->setAlignment(Qt::AlignCenter);

    tableWidget = new QTableWidget(this);
    tableWidget->setColumnCount(4);
    tableWidget->setHorizontalHeaderLabels({"Date", "Nom", "Prénom", "Description"});
    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableWidget->setWordWrap(true);
    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableWidget->hide();  // Caché jusqu'à l'affichage

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(infoLabel);
    layout->addWidget(historiqueAlertesButton, 0, Qt::AlignCenter);
    layout->addWidget(historiqueAccesButton, 0, Qt::AlignCenter);
    layout->addWidget(listeEmployesButton, 0, Qt::AlignCenter);
    layout->addWidget(retourButton, 0, Qt::AlignCenter);
    layout->addWidget(tableWidget);
    setLayout(layout);

    connect(historiqueAlertesButton, &QPushButton::clicked, this, &Zone0::showHistoriqueAlertes);
    connect(historiqueAccesButton, &QPushButton::clicked, this, &Zone0::showHistoriqueAcces);
    connect(listeEmployesButton, &QPushButton::clicked, this, &Zone0::showListeEmployes);
    connect(retourButton, &QPushButton::clicked, this, &Zone0::retour);

    // Timer pour rafraîchir les alertes toutes les 5 secondes
    refreshAlertesTimer = new QTimer(this);
    connect(refreshAlertesTimer, &QTimer::timeout, this, &Zone0::updateHistoriqueAlertes);
    refreshAlertesTimer->start(1000); // Rafraîchit toutes les 5 secondes

    // Timer pour rafraîchir les accès toutes les 5 secondes
    refreshAccesTimer = new QTimer(this);
    connect(refreshAccesTimer, &QTimer::timeout, this, &Zone0::updateHistoriqueAcces);
    refreshAccesTimer->start(1000); // Rafraîchit toutes les 5 secondes
}

void Zone0::setupDatabase()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("localhost");
    db.setDatabaseName("Datacenter_v1");
    db.setUserName("anous");
    db.setPassword("Anousa@2006");

    if (!db.open()) {
        QMessageBox::critical(this, "Erreur", "Impossible de se connecter à la base de données.");
    }
}

void Zone0::showHistoriqueAlertes()
{
    setupDatabase();

    // Désactiver le timer des accès et activer celui des alertes
    refreshAccesTimer->stop();
    refreshAlertesTimer->start(5000);

    QSqlQuery query;
    // Récupère uniquement la date et la description depuis la table alerte_zone0
    query.prepare("SELECT date_alerte, description FROM alerte_zone0 ORDER BY date_alerte DESC");

    if (!query.exec()) {
        QMessageBox::critical(this, "Erreur", "Échec de la requête : " + query.lastError().text());
        return;
    }

    // Configurer le tableau pour n'avoir que 2 colonnes
    tableWidget->clear();
    tableWidget->setColumnCount(2);
    tableWidget->setHorizontalHeaderLabels(QStringList() << "Date" << "Description");
    tableWidget->setRowCount(0);

    int row = 0;
    while (query.next()) {
        tableWidget->insertRow(row);
        // Colonne 0 : Date
        tableWidget->setItem(row, 0, new QTableWidgetItem(query.value(0).toString()));
        // Colonne 1 : Description
        tableWidget->setItem(row, 1, new QTableWidgetItem(query.value(1).toString()));
        row++;
    }

    tableWidget->show();
}

void Zone0::updateHistoriqueAlertes()
{
    if (!isVisible()) return;

    QSqlQuery query;
    query.prepare("SELECT date_alerte, description FROM alerte_zone0 ORDER BY date_alerte DESC");

    if (!query.exec()) {
        return;
    }

    tableWidget->clear();
    tableWidget->setColumnCount(2);
    tableWidget->setHorizontalHeaderLabels(QStringList() << "Date" << "Description");
    tableWidget->setRowCount(0);

    int row = 0;
    while (query.next()) {
        tableWidget->insertRow(row);
        tableWidget->setItem(row, 0, new QTableWidgetItem(query.value(0).toString()));
        tableWidget->setItem(row, 1, new QTableWidgetItem(query.value(1).toString()));
        row++;
    }
}


void Zone0::showHistoriqueAcces()
{
    setupDatabase();

    // Désactiver le timer des alertes et activer celui des accès
    refreshAlertesTimer->stop();
    refreshAccesTimer->start(5000);

    QSqlQuery query;
    // Récupère uniquement la date et la description depuis la table acces_zone0
    query.prepare("SELECT date_acces, description FROM acces_zone0 ORDER BY date_acces DESC");

    if (!query.exec()) {
        QMessageBox::critical(this, "Erreur", "Échec de la requête : " + query.lastError().text());
        return;
    }

    // Configurer le tableau pour n'avoir que 2 colonnes
    tableWidget->clear();
    tableWidget->setColumnCount(2);
    tableWidget->setHorizontalHeaderLabels(QStringList() << "Date" << "Description");
    tableWidget->setRowCount(0);

    int row = 0;
    while (query.next()) {
        tableWidget->insertRow(row);
        // Colonne 0 : Date
        tableWidget->setItem(row, 0, new QTableWidgetItem(query.value(0).toString()));
        // Colonne 1 : Description
        tableWidget->setItem(row, 1, new QTableWidgetItem(query.value(1).toString()));
        row++;
    }

    tableWidget->show();
}

void Zone0::updateHistoriqueAcces()
{
    if (!isVisible()) return;

    QSqlQuery query;
    query.prepare("SELECT date_acces, description FROM acces_zone0 ORDER BY date_acces DESC");

    if (!query.exec()) {
        return;
    }

    tableWidget->clear();
    tableWidget->setColumnCount(2);
    tableWidget->setHorizontalHeaderLabels(QStringList() << "Date" << "Description");
    tableWidget->setRowCount(0);

    int row = 0;
    while (query.next()) {
        tableWidget->insertRow(row);
        tableWidget->setItem(row, 0, new QTableWidgetItem(query.value(0).toString()));
        tableWidget->setItem(row, 1, new QTableWidgetItem(query.value(1).toString()));
        row++;
    }
}

void Zone0::showListeEmployes()
{
    setupDatabase(); // Connexion à la base de données

    // Désactiver les timers pour éviter qu'un autre affichage prenne le dessus après 5s
    refreshAlertesTimer->stop();
    refreshAccesTimer->stop();

    QSqlQuery query;
    query.prepare("SELECT nom, prenom FROM employes_zone0 ORDER BY nom ASC");

    if (!query.exec()) {
        QMessageBox::critical(this, "Erreur", "Échec de la requête : " + query.lastError().text());
        return;
    }

    // Configuration du tableau
    tableWidget->clear();  // Nettoyer les anciennes données
    tableWidget->setColumnCount(2);
    tableWidget->setHorizontalHeaderLabels({"Nom", "Prénom"});
    tableWidget->setRowCount(0);

    int row = 0;
    while (query.next()) {
        tableWidget->insertRow(row);
        tableWidget->setItem(row, 0, new QTableWidgetItem(query.value(0).toString()));  // Nom
        tableWidget->setItem(row, 1, new QTableWidgetItem(query.value(1).toString()));  // Prénom
        row++;
    }

    tableWidget->show();  // Afficher le tableau
}

void Zone0::updateListeEmployes()
{
    if (!isVisible()) return;  // Éviter les mises à jour inutiles si la fenêtre est cachée

    QSqlQuery query;
    query.prepare("SELECT nom, prenom FROM employes_zone0 ORDER BY nom ASC");

    if (!query.exec()) {
        return;  // Éviter d'afficher une erreur à chaque échec de requête
    }

    // Mettre à jour la table
    tableWidget->clear();
    tableWidget->setColumnCount(2);
    tableWidget->setHorizontalHeaderLabels({"Nom", "Prénom"});
    tableWidget->setRowCount(0);

    int row = 0;
    while (query.next()) {
        tableWidget->insertRow(row);
        tableWidget->setItem(row, 0, new QTableWidgetItem(query.value(0).toString()));  // Nom
        tableWidget->setItem(row, 1, new QTableWidgetItem(query.value(1).toString()));  // Prénom
        row++;
    }

    tableWidget->show();
}


void Zone0::retour()
{
    this->close();
    menu = new Menu();
    menu->show();
}
*/

#include "zone0.h"
#include "menu.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QTimer>
#include <QHeaderView>
#include <QGraphicsDropShadowEffect>

Zone0::Zone0(QWidget *parent) : QWidget(parent)
{
    setWindowTitle("Zone 0");
    resize(700, 300);  // Augmenté la taille

    // Style global
    setStyleSheet(R"(
        QPushButton {
            background-color: #3498db;
            color: white;
            border-radius: 10px;
            font-weight: bold;
            font-size: 14px;
            padding: 10px;
            transition: all 0.3s ease;
            border: 2px solid #2980b9;
            box-shadow: 0 4px 6px rgba(0,0,0,0.1);
        }
        QPushButton:hover {
            background-color: #2980b9;
            transform: scale(1.05);
            box-shadow: 0 6px 8px rgba(0,0,0,0.2);
        }
        QPushButton:pressed {
            background-color: #21618C;
            transform: scale(0.95);
        }
        QLabel {
            font-size: 16px;
            font-weight: bold;
            color: #2c3e50;
            margin-bottom: 20px;
        }
        QTableWidget {
            background-color: #f0f4f8;
            alternate-background-color: #e6eaf0;
            selection-background-color: #3498db;
            border-radius: 10px;
            padding: 10px;
        }
        QHeaderView::section {
            background-color: #3498db;
            color: white;
            padding: 5px;
            border: none;
            font-weight: bold;
        }
    )");

    // Création des boutons
    QPushButton *historiqueAlertesButton = new QPushButton("Historique des alertes");
    QPushButton *historiqueAccesButton = new QPushButton("Historique des accès");
    QPushButton *listeEmployesButton = new QPushButton("Liste des employés ayant accès");
    QPushButton *retourButton = new QPushButton("Retour");

    // Liste des boutons pour traitement en masse
    QList<QPushButton*> boutons = {
        historiqueAlertesButton,
        historiqueAccesButton,
        listeEmployesButton,
        retourButton
    };

    // Configuration des boutons
    for (QPushButton *bouton : boutons) {
        // Taille fixe
        bouton->setFixedSize(350, 50);  // Légèrement plus large

        // Effet d'ombre personnalisé
        QGraphicsDropShadowEffect* shadowEffect = new QGraphicsDropShadowEffect(bouton);
        shadowEffect->setBlurRadius(15);
        shadowEffect->setColor(QColor(0, 0, 0, 50));
        shadowEffect->setOffset(3, 3);
        bouton->setGraphicsEffect(shadowEffect);
    }

    // Couleurs spécifiques pour chaque bouton
    historiqueAlertesButton->setStyleSheet(historiqueAlertesButton->styleSheet() +
                                           "background-color: darkblue; border-color: darkblue;");

    historiqueAccesButton->setStyleSheet(historiqueAccesButton->styleSheet() +
                                         "background-color: darkblue; border-color: darkblue;");

    listeEmployesButton->setStyleSheet(listeEmployesButton->styleSheet() +
                                       "background-color: darkblue; border-color: darkblue;");

    retourButton->setStyleSheet(retourButton->styleSheet() +
                                "background-color: #34495e; border-color: #2c3e50;");

    // Label d'information
    infoLabel = new QLabel("Sélectionnez une option");
    infoLabel->setAlignment(Qt::AlignCenter);

    // Configuration du tableau
    tableWidget = new QTableWidget(this);
    tableWidget->setColumnCount(4);
    tableWidget->setHorizontalHeaderLabels({"Date", "Nom", "Prénom", "Description"});
    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableWidget->setAlternatingRowColors(true);
    tableWidget->setWordWrap(true);
    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableWidget->hide();  // Caché jusqu'à l'affichage

    // Layout principal
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setSpacing(15);  // Espacement entre les éléments
    layout->addWidget(infoLabel);

    // Sous-layout pour centrer les boutons
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    QVBoxLayout *buttonVerticalLayout = new QVBoxLayout;
    buttonVerticalLayout->addWidget(historiqueAlertesButton);
    buttonVerticalLayout->addWidget(historiqueAccesButton);
    buttonVerticalLayout->addWidget(listeEmployesButton);
    buttonVerticalLayout->addWidget(retourButton);
    buttonVerticalLayout->setAlignment(Qt::AlignCenter);

    buttonLayout->addStretch();
    buttonLayout->addLayout(buttonVerticalLayout);
    buttonLayout->addStretch();

    layout->addLayout(buttonLayout);
    layout->addWidget(tableWidget);

    setLayout(layout);

    // Connexion des signaux
    connect(historiqueAlertesButton, &QPushButton::clicked, this, &Zone0::showHistoriqueAlertes);
    connect(historiqueAccesButton, &QPushButton::clicked, this, &Zone0::showHistoriqueAcces);
    connect(listeEmployesButton, &QPushButton::clicked, this, &Zone0::showListeEmployes);
    connect(retourButton, &QPushButton::clicked, this, &Zone0::retour);

    // Timers pour rafraîchissement
    refreshAlertesTimer = new QTimer(this);
    connect(refreshAlertesTimer, &QTimer::timeout, this, &Zone0::updateHistoriqueAlertes);
    refreshAlertesTimer->start(5000); // Rafraîchit toutes les 5 secondes

    refreshAccesTimer = new QTimer(this);
    connect(refreshAccesTimer, &QTimer::timeout, this, &Zone0::updateHistoriqueAcces);
    refreshAccesTimer->start(5000); // Rafraîchit toutes les 5 secondes
}
void Zone0::setupDatabase()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("localhost");
    db.setDatabaseName("Datacenter_v1");
    db.setUserName("anous");
    db.setPassword("Anousa@2006");

    if (!db.open()) {
        QMessageBox::critical(this, "Erreur", "Impossible de se connecter à la base de données.");
    }
}

void Zone0::showHistoriqueAlertes()
{
    setupDatabase();

    // Désactiver le timer des accès et activer celui des alertes
    refreshAccesTimer->stop();
    refreshAlertesTimer->start(5000);

    QSqlQuery query;
    // Récupère uniquement la date et la description depuis la table alerte_zone0
    query.prepare("SELECT date_alerte, description FROM alerte_zone0 ORDER BY date_alerte DESC");

    if (!query.exec()) {
        QMessageBox::critical(this, "Erreur", "Échec de la requête : " + query.lastError().text());
        return;
    }

    // Configurer le tableau pour n'avoir que 2 colonnes
    tableWidget->clear();
    tableWidget->setColumnCount(2);
    tableWidget->setHorizontalHeaderLabels(QStringList() << "Date" << "Description");
    tableWidget->setRowCount(0);

    int row = 0;
    while (query.next()) {
        tableWidget->insertRow(row);
        // Colonne 0 : Date
        tableWidget->setItem(row, 0, new QTableWidgetItem(query.value(0).toString()));
        // Colonne 1 : Description
        tableWidget->setItem(row, 1, new QTableWidgetItem(query.value(1).toString()));
        row++;
    }

    tableWidget->show();
}

void Zone0::updateHistoriqueAlertes()
{
    if (!isVisible()) return;

    QSqlQuery query;
    query.prepare("SELECT date_alerte, description FROM alerte_zone0 ORDER BY date_alerte DESC");

    if (!query.exec()) {
        return;
    }

    tableWidget->clear();
    tableWidget->setColumnCount(2);
    tableWidget->setHorizontalHeaderLabels(QStringList() << "Date" << "Description");
    tableWidget->setRowCount(0);

    int row = 0;
    while (query.next()) {
        tableWidget->insertRow(row);
        tableWidget->setItem(row, 0, new QTableWidgetItem(query.value(0).toString()));
        tableWidget->setItem(row, 1, new QTableWidgetItem(query.value(1).toString()));
        row++;
    }
}


void Zone0::showHistoriqueAcces()
{
    setupDatabase();

    // Désactiver le timer des alertes et activer celui des accès
    refreshAlertesTimer->stop();
    refreshAccesTimer->start(5000);

    QSqlQuery query;
    // Récupère uniquement la date et la description depuis la table acces_zone0
    query.prepare("SELECT date_acces, description FROM acces_zone0 ORDER BY date_acces DESC");

    if (!query.exec()) {
        QMessageBox::critical(this, "Erreur", "Échec de la requête : " + query.lastError().text());
        return;
    }

    // Configurer le tableau pour n'avoir que 2 colonnes
    tableWidget->clear();
    tableWidget->setColumnCount(2);
    tableWidget->setHorizontalHeaderLabels(QStringList() << "Date" << "Description");
    tableWidget->setRowCount(0);

    int row = 0;
    while (query.next()) {
        tableWidget->insertRow(row);
        // Colonne 0 : Date
        tableWidget->setItem(row, 0, new QTableWidgetItem(query.value(0).toString()));
        // Colonne 1 : Description
        tableWidget->setItem(row, 1, new QTableWidgetItem(query.value(1).toString()));
        row++;
    }

    tableWidget->show();
}

void Zone0::updateHistoriqueAcces()
{
    if (!isVisible()) return;

    QSqlQuery query;
    query.prepare("SELECT date_acces, description FROM acces_zone0 ORDER BY date_acces DESC");

    if (!query.exec()) {
        return;
    }

    tableWidget->clear();
    tableWidget->setColumnCount(2);
    tableWidget->setHorizontalHeaderLabels(QStringList() << "Date" << "Description");
    tableWidget->setRowCount(0);

    int row = 0;
    while (query.next()) {
        tableWidget->insertRow(row);
        tableWidget->setItem(row, 0, new QTableWidgetItem(query.value(0).toString()));
        tableWidget->setItem(row, 1, new QTableWidgetItem(query.value(1).toString()));
        row++;
    }
}

void Zone0::showListeEmployes()
{
    setupDatabase(); // Connexion à la base de données

    // Désactiver les timers pour éviter qu'un autre affichage prenne le dessus après 5s
    refreshAlertesTimer->stop();
    refreshAccesTimer->stop();

    QSqlQuery query;
    query.prepare("SELECT nom, prenom FROM employes_zone0 ORDER BY nom ASC");

    if (!query.exec()) {
        QMessageBox::critical(this, "Erreur", "Échec de la requête : " + query.lastError().text());
        return;
    }

    // Configuration du tableau
    tableWidget->clear();  // Nettoyer les anciennes données
    tableWidget->setColumnCount(2);
    tableWidget->setHorizontalHeaderLabels({"Nom", "Prénom"});
    tableWidget->setRowCount(0);

    int row = 0;
    while (query.next()) {
        tableWidget->insertRow(row);
        tableWidget->setItem(row, 0, new QTableWidgetItem(query.value(0).toString()));  // Nom
        tableWidget->setItem(row, 1, new QTableWidgetItem(query.value(1).toString()));  // Prénom
        row++;
    }

    tableWidget->show();  // Afficher le tableau
}

void Zone0::updateListeEmployes()
{
    if (!isVisible()) return;  // Éviter les mises à jour inutiles si la fenêtre est cachée

    QSqlQuery query;
    query.prepare("SELECT nom, prenom FROM employes_zone0 ORDER BY nom ASC");

    if (!query.exec()) {
        return;  // Éviter d'afficher une erreur à chaque échec de requête
    }

    // Mettre à jour la table
    tableWidget->clear();
    tableWidget->setColumnCount(2);
    tableWidget->setHorizontalHeaderLabels({"Nom", "Prénom"});
    tableWidget->setRowCount(0);

    int row = 0;
    while (query.next()) {
        tableWidget->insertRow(row);
        tableWidget->setItem(row, 0, new QTableWidgetItem(query.value(0).toString()));  // Nom
        tableWidget->setItem(row, 1, new QTableWidgetItem(query.value(1).toString()));  // Prénom
        row++;
    }

    tableWidget->show();
}


void Zone0::retour()
{
    this->close();
    menu = new Menu();
    menu->show();
}
