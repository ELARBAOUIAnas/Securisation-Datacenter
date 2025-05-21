/*#include "menu.h"
#include "connexion1.h"
#include "zone0.h"
#include "zone1.h"
#include "zone2.h"
#include "zone3.h"
#include <QMessageBox>
#include <QApplication>
#include <QPointer>
#include <QLabel>

Menu::Menu(QWidget *parent) : QWidget(parent)
{
    setWindowTitle("Menu");
    resize(700, 300);
    infoLabel = new QLabel();
    infoLabel->setAlignment(Qt::AlignCenter);

    // Création du layout principal
    QVBoxLayout *Pagelayout = new QVBoxLayout(this);

    // Création des layouts secondaires
    QHBoxLayout *Hlayout1 = new QHBoxLayout;
    QHBoxLayout *Hlayout2 = new QHBoxLayout;
    QVBoxLayout *Hlayout3 = new QVBoxLayout;
    QVBoxLayout *Hlayout4 = new QVBoxLayout;

    // Ajout des sous-layouts au layout principal
    Pagelayout->addLayout(Hlayout1);
    Pagelayout->addLayout(Hlayout2);
    Pagelayout->addLayout(Hlayout3);
    Pagelayout->addLayout(Hlayout4);

    // Création des boutons
    QPushButton *zone0Button = new QPushButton("Zone 0");
    QPushButton *zone1Button = new QPushButton("Zone 1");
    QPushButton *zone2Button = new QPushButton("Zone 2");
    QPushButton *zone3Button = new QPushButton("Zone 3");
    QPushButton *deconnexionButton = new QPushButton("Déconnexion");
    QPushButton *quitterButton = new QPushButton("Quitter");

    // Définition de la taille des boutons
    QList<QPushButton*> boutons = {zone0Button, zone1Button, zone2Button, zone3Button, deconnexionButton, quitterButton};
    for (QPushButton *bouton : boutons) {
        bouton->setFixedSize(300, 40);
    }

    // Ajout des boutons aux sous-layouts
    Pagelayout->addWidget(infoLabel);

    Hlayout1->addWidget(zone0Button);
    Hlayout1->addWidget(zone1Button);
    Hlayout2->addWidget(zone2Button);
    Hlayout2->addWidget(zone3Button);
    Hlayout3->addWidget(deconnexionButton, 0, Qt::AlignCenter);
    Hlayout4->addWidget(quitterButton, 0, Qt::AlignCenter);

    // Connexion des signaux aux slots
    connect(zone0Button, &QPushButton::clicked, this, &Menu::zoneWin0);
    connect(zone1Button, &QPushButton::clicked, this, &Menu::zoneWin1);
    connect(zone2Button, &QPushButton::clicked, this, &Menu::zoneWin2);
    connect(zone3Button, &QPushButton::clicked, this, &Menu::zoneWin3);
    connect(deconnexionButton, &QPushButton::clicked, this, &Menu::deconnexion);
    connect(quitterButton, &QPushButton::clicked, this, &Menu::quitter);
}

void Menu::zoneWin0(){
    this->close();
    Zone0 *zone0Win = new Zone0();
    zone0Win->show();
}
void Menu::zoneWin1(){
    this->close();
    Zone1 *zone1Win = new Zone1();
    zone1Win->show();
}
void Menu::zoneWin2(){
    this->close();
    Zone2 *zone2Win = new Zone2();
    zone2Win->show();
}

void Menu::zoneWin3(){
    this->close();
    Zone3 *zone3Win = new Zone3();
    zone3Win->show();
}

void Menu::deconnexion()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Déconnexion", "Voulez-vous vraiment vous déconnecter ?", QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        this->close(); // Fermer la fenêtre du menu

        // Ouvrir la fenêtre de connexion de manière sécurisée
        QPointer<connexion1> connexion12 = new connexion1();
        connexion12->show();
    }
}

void Menu::quitter()
{
    QMessageBox::StandardButton reply = QMessageBox::question(this, "Quitter", "Êtes-vous sûr de vouloir quitter ?", QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        qApp->quit();
    }
}*/

// menu.cpp
#include "menu.h"
#include <QMessageBox>
#include <QApplication>
#include <QPointer>
#include <QGraphicsDropShadowEffect>
#include <QPropertyAnimation>

#include "connexion1.h"
#include "zone0.h"
#include "zone1.h"
#include "zone2.h"
#include "zone3.h"

Menu::Menu(QWidget *parent) : QMainWindow(parent)
{
    // Créer un widget central
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    // Style global avec animations
    setStyleSheet(R"(
        QMainWindow {
            background-color: #f0f4f8;
        }
        QPushButton {
            background-color: #3498db;
            color: white;
            border-radius: 8px;
            font-weight: bold;
            transition: all 0.3s ease;
            padding: 10px;
        }
        QPushButton:hover {
            background-color: #2980b9;
            transform: scale(1.05);
        }
        QPushButton:pressed {
            background-color: #21618C;
            transform: scale(0.95);
        }
        QLabel {
            color: #2c3e50;
            font-size: 16px;
            font-weight: bold;
        }
    )");

    setWindowTitle("Menu");
    resize(700, 300);  // Augmenté la hauteur

    // Création d'effets visuels
    QGraphicsDropShadowEffect* shadowEffect = new QGraphicsDropShadowEffect(this);
    shadowEffect->setBlurRadius(15);
    shadowEffect->setColor(QColor(0, 0, 0, 50));
    shadowEffect->setOffset(3, 3);
    centralWidget->setGraphicsEffect(shadowEffect);

    // Création du layout principal
    QVBoxLayout *Pagelayout = new QVBoxLayout(centralWidget);

    // Création du label d'information
    infoLabel = new QLabel("Bienvenue dans votre application");
    infoLabel->setAlignment(Qt::AlignCenter);
    Pagelayout->addWidget(infoLabel);

    // Création des layouts secondaires
    QHBoxLayout *Hlayout1 = new QHBoxLayout;
    QHBoxLayout *Hlayout2 = new QHBoxLayout;
    QVBoxLayout *Hlayout3 = new QVBoxLayout;
    QVBoxLayout *Hlayout4 = new QVBoxLayout;

    // Ajout des sous-layouts au layout principal
    Pagelayout->addLayout(Hlayout1);
    Pagelayout->addLayout(Hlayout2);
    Pagelayout->addLayout(Hlayout3);
    Pagelayout->addLayout(Hlayout4);

    // Création des boutons
    QPushButton *zone0Button = new QPushButton("Zone 0");
    QPushButton *zone1Button = new QPushButton("Zone 1");
    QPushButton *zone2Button = new QPushButton("Zone 2");
    QPushButton *zone3Button = new QPushButton("Zone 3");
    QPushButton *deconnexionButton = new QPushButton("Déconnexion");
    QPushButton *quitterButton = new QPushButton("Quitter");

    // Définition de la taille des boutons
    QList<QPushButton*> boutons = {zone0Button, zone1Button, zone2Button, zone3Button, deconnexionButton, quitterButton};
    for (QPushButton *bouton : boutons) {
        bouton->setFixedSize(300, 40);

        // Ajouter un effet de shadow à chaque bouton
        QGraphicsDropShadowEffect* buttonShadow = new QGraphicsDropShadowEffect(bouton);
        buttonShadow->setBlurRadius(10);
        buttonShadow->setColor(QColor(0, 0, 0, 30));
        buttonShadow->setOffset(2, 2);
        bouton->setGraphicsEffect(buttonShadow);
    }

    // Personnalisation des couleurs par zone
    zone0Button->setStyleSheet("background-color: darkblue;");
    zone1Button->setStyleSheet("background-color: darkblue;");
    zone2Button->setStyleSheet("background-color: darkblue;");
    zone3Button->setStyleSheet("background-color: darkblue  ;");
    deconnexionButton->setStyleSheet("background-color: #9b59b6;");
    quitterButton->setStyleSheet("background-color: #34495e;");

    // Ajout des boutons aux sous-layouts
    Hlayout1->addWidget(zone0Button);
    Hlayout1->addWidget(zone1Button);
    Hlayout2->addWidget(zone2Button);
    Hlayout2->addWidget(zone3Button);
    Hlayout3->addWidget(deconnexionButton, 0, Qt::AlignCenter);
    Hlayout4->addWidget(quitterButton, 0, Qt::AlignCenter);

    // Connexion des signaux aux slots
    connect(zone0Button, &QPushButton::clicked, this, &Menu::zoneWin0);
    connect(zone1Button, &QPushButton::clicked, this, &Menu::zoneWin1);
    connect(zone2Button, &QPushButton::clicked, this, &Menu::zoneWin2);
    connect(zone3Button, &QPushButton::clicked, this, &Menu::zoneWin3);
    connect(deconnexionButton, &QPushButton::clicked, this, &Menu::deconnexion);
    connect(quitterButton, &QPushButton::clicked, this, &Menu::quitter);
}

void Menu::zoneWin0(){
    this->close();
    Zone0 *zone0Win = new Zone0();
    zone0Win->show();
}

void Menu::zoneWin1(){
    this->close();
    Zone1 *zone1Win = new Zone1();
    zone1Win->show();
}

void Menu::zoneWin2(){
    this->close();
    Zone2 *zone2Win = new Zone2();
    zone2Win->show();
}

void Menu::zoneWin3(){
    this->close();
    Zone3 *zone3Win = new Zone3();
    zone3Win->show();
}

void Menu::deconnexion()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Déconnexion", "Voulez-vous vraiment vous déconnecter ?", QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        this->close();
        connexion1 *connexion12 = new connexion1();
        connexion12->show();
    }
}

void Menu::quitter()
{
    QMessageBox::StandardButton reply = QMessageBox::question(this, "Quitter", "Êtes-vous sûr de vouloir quitter ?", QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        qApp->quit();
    }
}

