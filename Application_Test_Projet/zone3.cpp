/*
#include "zone3.h"
#include "menu.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QTimer>
#include <QHeaderView>
#include <QDebug>


Zone3::Zone3(QWidget *parent) : QWidget(parent)
{
    setWindowTitle("Zone 3");
    resize(600, 700);

    // Boutons de navigation
    QPushButton *historiqueAlertesButton = new QPushButton("Historique des alertes");
    QPushButton *historiqueAccesButton = new QPushButton("Historique des accès");
    QPushButton *listeEmployesButton = new QPushButton("Liste des employés ayant accès");
    QPushButton *employesBloquesButton = new QPushButton("Employés bloqués");  // Nouveau bouton
    QPushButton *retourButton = new QPushButton("Retour");

    historiqueAlertesButton->setFixedSize(300, 40);
    historiqueAccesButton->setFixedSize(300, 40);
    listeEmployesButton->setFixedSize(300, 40);
    employesBloquesButton->setFixedSize(300, 40);
    retourButton->setFixedSize(300, 40);

    infoLabel = new QLabel("Sélectionnez une option.");
    infoLabel->setAlignment(Qt::AlignCenter);

    tableWidget = new QTableWidget(this);
    // Configuration initiale – sera réinitialisée dans chaque fonction selon les besoins
    tableWidget->setColumnCount(4);
    tableWidget->setHorizontalHeaderLabels({"Col1", "Col2", "Col3", "Action"});
    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableWidget->setWordWrap(true);
    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableWidget->hide();  // Caché jusqu'à ce qu'une option soit sélectionnée

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(infoLabel);
    layout->addWidget(historiqueAlertesButton, 0, Qt::AlignCenter);
    layout->addWidget(historiqueAccesButton, 0, Qt::AlignCenter);
    layout->addWidget(listeEmployesButton, 0, Qt::AlignCenter);
    layout->addWidget(employesBloquesButton, 0, Qt::AlignCenter);
    layout->addWidget(retourButton, 0, Qt::AlignCenter);
    layout->addWidget(tableWidget);
    setLayout(layout);

    connect(historiqueAlertesButton, &QPushButton::clicked, this, &Zone3::showHistoriqueAlertes);
    connect(historiqueAccesButton, &QPushButton::clicked, this, &Zone3::showHistoriqueAcces);
    connect(listeEmployesButton, &QPushButton::clicked, this, &Zone3::showListeEmployes);
    connect(employesBloquesButton, &QPushButton::clicked, this, &Zone3::showEmployesBloques);
    connect(retourButton, &QPushButton::clicked, this, &Zone3::retour);

    // Timers pour rafraîchir les historiques (actualisation toutes les 5 secondes)
    refreshAlertesTimer = new QTimer(this);
    connect(refreshAlertesTimer, &QTimer::timeout, this, &Zone3::updateHistoriqueAlertes);
    refreshAlertesTimer->start(5000);

    refreshAccesTimer = new QTimer(this);
    connect(refreshAccesTimer, &QTimer::timeout, this, &Zone3::updateHistoriqueAcces);
    refreshAccesTimer->start(5000);
}

Zone3::~Zone3()
{
    // Destructeur
}
void Zone3::setupDatabase()
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

void Zone3::showHistoriqueAlertes()
{
    // Arrêter le timer des accès pour éviter toute interférence
    refreshAccesTimer->stop();
    refreshAlertesTimer->start(5000);

    setupDatabase();
    QSqlDatabase db = QSqlDatabase::database();
    QSqlQuery query(db);
    // On récupère seulement la date et la description pour les alertes
    QString sqlStr = "SELECT date_alerte, description FROM alerte_zone3 ORDER BY date_alerte DESC";
    qDebug() << "Exécution de la requête:" << sqlStr;
    if (!query.exec(sqlStr)) {
        QMessageBox::critical(this, "Erreur", "Échec de la requête : " + query.lastError().text());
        qDebug() << "Erreur de requête:" << query.lastError().text();
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
    tableWidget->show();
}

void Zone3::updateHistoriqueAlertes()
{
    if (!isVisible()) return;
    // On réutilise la même requête que showHistoriqueAlertes
    QSqlDatabase db = QSqlDatabase::database();
    QSqlQuery query(db);
    query.prepare("SELECT date_alerte, description FROM alerte_zone3 ORDER BY date_alerte DESC");
    if (!query.exec())
        return;

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

// ------------------------
// Affichage des accès
// ------------------------
void Zone3::showHistoriqueAcces()
{
    setupDatabase();
    QSqlDatabase db = QSqlDatabase::database();

    tableWidget->clear();
    tableWidget->setColumnCount(2);
    tableWidget->setHorizontalHeaderLabels(QStringList() << "Date" << "Description");

    refreshAlertesTimer->stop();
    refreshAccesTimer->start(5000);

    QSqlQuery query(db);
    // On récupère seulement la date et la description pour les accès
    query.prepare("SELECT date_acces, description FROM acces_zone3 ORDER BY date_acces DESC");
    if (!query.exec()) {
        QMessageBox::critical(this, "Erreur", "Échec de la requête : " + query.lastError().text());
        return;
    }

    tableWidget->setRowCount(0);
    int row = 0;
    while (query.next()) {
        tableWidget->insertRow(row);
        tableWidget->setItem(row, 0, new QTableWidgetItem(query.value(0).toString()));
        tableWidget->setItem(row, 1, new QTableWidgetItem(query.value(1).toString()));
        row++;
    }
    tableWidget->show();
}

void Zone3::updateHistoriqueAcces()
{
    if (!isVisible()) return;

    QSqlDatabase db = QSqlDatabase::database();
    QSqlQuery query(db);
    query.prepare("SELECT date_acces, description FROM acces_zone3 ORDER BY date_acces DESC");
    if (!query.exec())
        return;

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

// ------------------------
// Affichage de la liste des employés
// ------------------------
void Zone3::showListeEmployes()
{
    setupDatabase();
    QSqlDatabase db = QSqlDatabase::database();

    refreshAlertesTimer->stop();
    refreshAccesTimer->stop();

    QSqlQuery query(db);
    query.prepare("SELECT nom, prenom FROM employes_zone3 ORDER BY nom ASC");
    if (!query.exec()) {
        QMessageBox::critical(this, "Erreur", "Échec de la requête : " + query.lastError().text());
        return;
    }

    tableWidget->clear();
    tableWidget->setColumnCount(2);
    tableWidget->setHorizontalHeaderLabels(QStringList() << "Nom" << "Prénom");
    tableWidget->setRowCount(0);
    int row = 0;
    while (query.next()) {
        tableWidget->insertRow(row);
        tableWidget->setItem(row, 0, new QTableWidgetItem(query.value(0).toString()));
        tableWidget->setItem(row, 1, new QTableWidgetItem(query.value(1).toString()));
        row++;
    }
    tableWidget->show();
}

void Zone3::updateListeEmployes()
{
    if (!isVisible()) return;
    QSqlQuery query;
    query.prepare("SELECT nom, prenom FROM employes_zone3 ORDER BY nom ASC");
    if (!query.exec())
        return;
    tableWidget->clear();
    tableWidget->setColumnCount(2);
    tableWidget->setHorizontalHeaderLabels(QStringList() << "Nom" << "Prénom");
    tableWidget->setRowCount(0);
    int row = 0;
    while (query.next()) {
        tableWidget->insertRow(row);
        tableWidget->setItem(row, 0, new QTableWidgetItem(query.value(0).toString()));
        tableWidget->setItem(row, 1, new QTableWidgetItem(query.value(1).toString()));
        row++;
    }
    tableWidget->show();
}

void Zone3::showEmployesBloques()
{
    refreshAccesTimer->stop();
    refreshAlertesTimer->stop();
    setupDatabase();
    QSqlDatabase db = QSqlDatabase::database();

    QSqlQuery query;
    query.prepare("SELECT empreinte_id, nom, prenom, bloque FROM empreinte_Zone3 WHERE bloque = 1");


    if (!query.exec()) {
        qDebug() << "Erreur lors de l'exécution de la requête : " << query.lastError().text();
    } else {
        qDebug() << "Requête exécutée avec succès.";
    }


    tableWidget->clear();
    tableWidget->setColumnCount(4);
    tableWidget->setHorizontalHeaderLabels({"Emreinte", "Nom", "Prénom", "Action"});
    tableWidget->setRowCount(0);

    int row = 0;
    while (query.next()) {
        QString uid = query.value(0).toString();
        QString nom = query.value(1).toString();
        QString prenom = query.value(2).toString();
        int bloque = query.value(3).toInt(); // Correct pour `bloque`
        qDebug() << "Bloqué (corrigé) :" << bloque;
        qDebug() << "Employé bloqué trouvé:" << uid << nom << prenom;

        tableWidget->insertRow(row);
        tableWidget->setItem(row, 0, new QTableWidgetItem(uid));
        tableWidget->setItem(row, 1, new QTableWidgetItem(nom));
        tableWidget->setItem(row, 2, new QTableWidgetItem(prenom));

        // Vérifier si les cellules sont bien remplies
        if (tableWidget->item(row, 0) == nullptr) qDebug() << "Erreur : UID non ajouté";
        if (tableWidget->item(row, 1) == nullptr) qDebug() << "Erreur : Nom non ajouté";
        if (tableWidget->item(row, 2) == nullptr) qDebug() << "Erreur : Prénom non ajouté";

        QPushButton *btnDebloquer = new QPushButton("Débloquer");
        btnDebloquer->setProperty("uid", nom);
        connect(btnDebloquer, &QPushButton::clicked, this, &Zone3::debloquerEmployeFromButton);
        tableWidget->setCellWidget(row, 3, btnDebloquer);

        row++;
    }
    qDebug() << "Nombre d'employés bloqués affichés:" << row;


    // Si aucun employé n'est trouvé, on affiche un message dans le log
    if (row == 0) {
        qDebug() << "Aucun employé bloqué trouvé dans la requête.";
    }

    tableWidget->show();
}



void Zone3::debloquerEmployeFromButton()
{
    QPushButton *btn = qobject_cast<QPushButton*>(sender());
    if (!btn)
        return;
    QString nom = btn->property("uid").toString();
    qDebug() << "Demande de déblocage pour UID:" << nom;

    QSqlDatabase db = QSqlDatabase::database();
    QSqlQuery query(db);
    query.prepare("UPDATE empreinte_Zone3 SET tentatives_echouees = 0, bloque = 0 WHERE nom = :uid");
    query.bindValue(":uid", nom);

    if (query.exec()) {
        QMessageBox::information(this, "Débloquage", "Monsieur " + nom + " est débloqué avec succès !");
        showEmployesBloques();
    } else {
        QString err = query.lastError().text();
        QMessageBox::critical(this, "Erreur", "Erreur lors du déblocage : " + err);
        qDebug() << "Erreur lors du débloquage:" << err;
    }
}


void Zone3::retour()
{
    this->close();
    menu = new Menu();
    menu->show();
}

*/
#include "zone3.h"
#include "menu.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QTimer>
#include <QHeaderView>
#include <QDebug>
Zone3::Zone3(QWidget *parent) : QWidget(parent)
{
    setWindowTitle("Zone 3");
    resize(700, 400);

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

    // Boutons de navigation
    QPushButton *historiqueAlertesButton = new QPushButton("Historique des alertes");
    QPushButton *historiqueAccesButton = new QPushButton("Historique des accès");
    QPushButton *listeEmployesButton = new QPushButton("Liste des employés ayant accès");
    QPushButton *employesBloquesButton = new QPushButton("Employés bloqués");
    QPushButton *retourButton = new QPushButton("Retour");

    // Couleurs spécifiques pour chaque bouton
    historiqueAlertesButton->setStyleSheet("background-color: darkblue; border-color: darkblue;");
    historiqueAccesButton->setStyleSheet("background-color: darkblue; border-color: darkblue;");
    listeEmployesButton->setStyleSheet("background-color: darkblue; border-color: darkblue;");
    employesBloquesButton->setStyleSheet("background-color: darkblue; border-color: darkblue;");
    retourButton->setStyleSheet("background-color: #34495e; border-color: #2c3e50;");

    // Taille des boutons
    historiqueAlertesButton->setFixedSize(350, 50);
    historiqueAccesButton->setFixedSize(350, 50);
    listeEmployesButton->setFixedSize(350, 50);
    employesBloquesButton->setFixedSize(350, 50);
    retourButton->setFixedSize(350, 50);

    infoLabel = new QLabel("Sélectionnez une option.");
    infoLabel->setAlignment(Qt::AlignCenter);
    infoLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: #2c3e50;");

    tableWidget = new QTableWidget(this);
    tableWidget->setColumnCount(4);
    tableWidget->setHorizontalHeaderLabels({"Col1", "Col2", "Col3", "Action"});
    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableWidget->setWordWrap(true);
    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableWidget->hide();

    // Layout principal avec centrage
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(infoLabel, 0, Qt::AlignCenter);
    layout->addWidget(historiqueAlertesButton, 0, Qt::AlignCenter);
    layout->addWidget(historiqueAccesButton, 0, Qt::AlignCenter);
    layout->addWidget(listeEmployesButton, 0, Qt::AlignCenter);
    layout->addWidget(employesBloquesButton, 0, Qt::AlignCenter);
    layout->addWidget(retourButton, 0, Qt::AlignCenter);
    layout->addWidget(tableWidget);
    setLayout(layout);

    // Le reste du code reste identique
    connect(historiqueAlertesButton, &QPushButton::clicked, this, &Zone3::showHistoriqueAlertes);
    connect(historiqueAccesButton, &QPushButton::clicked, this, &Zone3::showHistoriqueAcces);
    connect(listeEmployesButton, &QPushButton::clicked, this, &Zone3::showListeEmployes);
    connect(employesBloquesButton, &QPushButton::clicked, this, &Zone3::showEmployesBloques);
    connect(retourButton, &QPushButton::clicked, this, &Zone3::retour);

    // Timers pour rafraîchir les historiques (actualisation toutes les 5 secondes)
    refreshAlertesTimer = new QTimer(this);
    connect(refreshAlertesTimer, &QTimer::timeout, this, &Zone3::updateHistoriqueAlertes);
    refreshAlertesTimer->start(5000);

    refreshAccesTimer = new QTimer(this);
    connect(refreshAccesTimer, &QTimer::timeout, this, &Zone3::updateHistoriqueAcces);
    refreshAccesTimer->start(5000);
}
Zone3::~Zone3()
{
    // Destructeur
}
void Zone3::setupDatabase()
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

void Zone3::showHistoriqueAlertes()
{
    // Arrêter le timer des accès pour éviter toute interférence
    refreshAccesTimer->stop();
    refreshAlertesTimer->start(5000);

    setupDatabase();
    QSqlDatabase db = QSqlDatabase::database();
    QSqlQuery query(db);
    // On récupère seulement la date et la description pour les alertes
    QString sqlStr = "SELECT date_alerte, description FROM alerte_zone3 ORDER BY date_alerte DESC";
    qDebug() << "Exécution de la requête:" << sqlStr;
    if (!query.exec(sqlStr)) {
        QMessageBox::critical(this, "Erreur", "Échec de la requête : " + query.lastError().text());
        qDebug() << "Erreur de requête:" << query.lastError().text();
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
    tableWidget->show();
}

void Zone3::updateHistoriqueAlertes()
{
    if (!isVisible()) return;
    // On réutilise la même requête que showHistoriqueAlertes
    QSqlDatabase db = QSqlDatabase::database();
    QSqlQuery query(db);
    query.prepare("SELECT date_alerte, description FROM alerte_zone3 ORDER BY date_alerte DESC");
    if (!query.exec())
        return;

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

// ------------------------
// Affichage des accès
// ------------------------
void Zone3::showHistoriqueAcces()
{
    setupDatabase();
    QSqlDatabase db = QSqlDatabase::database();

    tableWidget->clear();
    tableWidget->setColumnCount(2);
    tableWidget->setHorizontalHeaderLabels(QStringList() << "Date" << "Description");

    refreshAlertesTimer->stop();
    refreshAccesTimer->start(5000);

    QSqlQuery query(db);
    // On récupère seulement la date et la description pour les accès
    query.prepare("SELECT date_acces, description FROM acces_zone3 ORDER BY date_acces DESC");
    if (!query.exec()) {
        QMessageBox::critical(this, "Erreur", "Échec de la requête : " + query.lastError().text());
        return;
    }

    tableWidget->setRowCount(0);
    int row = 0;
    while (query.next()) {
        tableWidget->insertRow(row);
        tableWidget->setItem(row, 0, new QTableWidgetItem(query.value(0).toString()));
        tableWidget->setItem(row, 1, new QTableWidgetItem(query.value(1).toString()));
        row++;
    }
    tableWidget->show();
}

void Zone3::updateHistoriqueAcces()
{
    if (!isVisible()) return;

    QSqlDatabase db = QSqlDatabase::database();
    QSqlQuery query(db);
    query.prepare("SELECT date_acces, description FROM acces_zone3 ORDER BY date_acces DESC");
    if (!query.exec())
        return;

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

// ------------------------
// Affichage de la liste des employés
// ------------------------
void Zone3::showListeEmployes()
{
    setupDatabase();
    QSqlDatabase db = QSqlDatabase::database();

    refreshAlertesTimer->stop();
    refreshAccesTimer->stop();

    QSqlQuery query(db);
    query.prepare("SELECT nom, prenom FROM employes_zone3 ORDER BY nom ASC");
    if (!query.exec()) {
        QMessageBox::critical(this, "Erreur", "Échec de la requête : " + query.lastError().text());
        return;
    }

    tableWidget->clear();
    tableWidget->setColumnCount(2);
    tableWidget->setHorizontalHeaderLabels(QStringList() << "Nom" << "Prénom");
    tableWidget->setRowCount(0);
    int row = 0;
    while (query.next()) {
        tableWidget->insertRow(row);
        tableWidget->setItem(row, 0, new QTableWidgetItem(query.value(0).toString()));
        tableWidget->setItem(row, 1, new QTableWidgetItem(query.value(1).toString()));
        row++;
    }
    tableWidget->show();
}

void Zone3::updateListeEmployes()
{
    if (!isVisible()) return;
    QSqlQuery query;
    query.prepare("SELECT nom, prenom FROM employes_zone3 ORDER BY nom ASC");
    if (!query.exec())
        return;
    tableWidget->clear();
    tableWidget->setColumnCount(2);
    tableWidget->setHorizontalHeaderLabels(QStringList() << "Nom" << "Prénom");
    tableWidget->setRowCount(0);
    int row = 0;
    while (query.next()) {
        tableWidget->insertRow(row);
        tableWidget->setItem(row, 0, new QTableWidgetItem(query.value(0).toString()));
        tableWidget->setItem(row, 1, new QTableWidgetItem(query.value(1).toString()));
        row++;
    }
    tableWidget->show();
}

void Zone3::showEmployesBloques()
{
    refreshAccesTimer->stop();
    refreshAlertesTimer->stop();
    setupDatabase();
    QSqlDatabase db = QSqlDatabase::database();

    QSqlQuery query;
    query.prepare("SELECT empreinte_id, nom, prenom, bloque FROM empreinte_Zone3 WHERE bloque = 1");


    if (!query.exec()) {
        qDebug() << "Erreur lors de l'exécution de la requête : " << query.lastError().text();
    } else {
        qDebug() << "Requête exécutée avec succès.";
    }


    tableWidget->clear();
    tableWidget->setColumnCount(4);
    tableWidget->setHorizontalHeaderLabels({"Emreinte", "Nom", "Prénom", "Action"});
    tableWidget->setRowCount(0);

    int row = 0;
    while (query.next()) {
        QString uid = query.value(0).toString();
        QString nom = query.value(1).toString();
        QString prenom = query.value(2).toString();
        int bloque = query.value(3).toInt(); // Correct pour `bloque`
        qDebug() << "Bloqué (corrigé) :" << bloque;
        qDebug() << "Employé bloqué trouvé:" << uid << nom << prenom;

        tableWidget->insertRow(row);
        tableWidget->setItem(row, 0, new QTableWidgetItem(uid));
        tableWidget->setItem(row, 1, new QTableWidgetItem(nom));
        tableWidget->setItem(row, 2, new QTableWidgetItem(prenom));

        // Vérifier si les cellules sont bien remplies
        if (tableWidget->item(row, 0) == nullptr) qDebug() << "Erreur : UID non ajouté";
        if (tableWidget->item(row, 1) == nullptr) qDebug() << "Erreur : Nom non ajouté";
        if (tableWidget->item(row, 2) == nullptr) qDebug() << "Erreur : Prénom non ajouté";

        QPushButton *btnDebloquer = new QPushButton("Débloquer");
        btnDebloquer->setStyleSheet("font-size: 12px; padding: 4px;");
        btnDebloquer->setProperty("uid", nom);
        connect(btnDebloquer, &QPushButton::clicked, this, &Zone3::debloquerEmployeFromButton);
        tableWidget->setCellWidget(row, 3, btnDebloquer);

        row++;
    }
    qDebug() << "Nombre d'employés bloqués affichés:" << row;


    // Si aucun employé n'est trouvé, on affiche un message dans le log
    if (row == 0) {
        qDebug() << "Aucun employé bloqué trouvé dans la requête.";
    }

    tableWidget->show();
}



void Zone3::debloquerEmployeFromButton()
{
    QPushButton *btn = qobject_cast<QPushButton*>(sender());
    if (!btn)
        return;
    QString nom = btn->property("uid").toString();
    qDebug() << "Demande de déblocage pour UID:" << nom;

    QSqlDatabase db = QSqlDatabase::database();
    QSqlQuery query(db);
    query.prepare("UPDATE empreinte_Zone3 SET tentatives_echouees = 0, bloque = 0 WHERE nom = :uid");
    query.bindValue(":uid", nom);

    if (query.exec()) {
        QMessageBox::information(this, "Débloquage", "Monsieur " + nom + " est débloqué avec succès !");
        showEmployesBloques();
    } else {
        QString err = query.lastError().text();
        QMessageBox::critical(this, "Erreur", "Erreur lors du déblocage : " + err);
        qDebug() << "Erreur lors du débloquage:" << err;
    }
}


void Zone3::retour()
{
    this->close();
    menu = new Menu();
    menu->show();
}
