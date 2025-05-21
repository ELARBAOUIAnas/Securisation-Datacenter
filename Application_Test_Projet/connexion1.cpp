
#include "connexion1.h"
#include "menu.h"
#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QMessageBox>
#include <QTimer>
#include <QDebug>
#include <QPixmap>
#include <QKeyEvent>

connexion1::connexion1(QWidget *parent)
    : QWidget(parent)
{
    QPixmap bkgnd("/home/anous/Images/DC.jpg");
    bkgnd = bkgnd.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Window, bkgnd);
    this->setPalette(palette);

    setWindowTitle("Connexion");
    resize(600, 300);
    connectToDatabase();

    // Création des éléments
    QLabel *idLabel = new QLabel(tr("Identifiant:"));
    idLabel->setStyleSheet("QLabel { font-size: 15px; color: white}");
    QLabel *passwordLabel = new QLabel(tr("Mot de passe:"));
    passwordLabel->setStyleSheet("QLabel { font-size: 15px; color: white }");

    idLineEdit = new QLineEdit;
    idLineEdit->setFixedSize(580, 30);
    passwordLineEdit = new QLineEdit;
    passwordLineEdit->setFixedSize(580, 30);
    passwordLineEdit->setEchoMode(QLineEdit::Password); // Masquer le mot de passe par défaut

    // Limiter les champs de saisie à 9 caractères
    idLineEdit->setMaxLength(9);
    passwordLineEdit->setMaxLength(9);
    //passwordCheckBox = new QCheckBox(tr("Masquer/afficher le mot de passe"));
    passwordCheckBox = new QCheckBox(tr("Masquer/afficher le mot de passe"));
    passwordCheckBox->setStyleSheet(
        "QCheckBox {"
        "    color: white;"  // Définit la couleur du texte en blanc
        "}"
        "QCheckBox::indicator {"
        "    width: 16px;"
        "    height: 16px;"
        "    border: 2px solid black;"  // Bordure noire
        "    background-color: white;"  // Fond blanc //
        "}"
        "QCheckBox::indicator:checked {"
        "    background-color: white;"  // Fond reste blanc //
        "    image: url(/home/anous/Images/coche1.jpg);" // Image d'une coche noire //
        "}"
        );






    //passwordCheckBox->setStyleSheet("color: white;");
    loginButton = new QPushButton(tr("Connexion"));
    loginButton->setShortcut(QKeySequence(Qt::Key_Return)); // Associer la touche "Entrée"

    loginButton->setFixedSize(300, 40);
    loginButton->setEnabled(false); // Désactivé au départ

    messageLabel = new QLabel;

    // Création du pavé numérique
    QGridLayout *numPadLayout = new QGridLayout;
    for (int i = 0; i <= 9; ++i) {
        QPushButton *button = new QPushButton(QString::number(i));
        button->setFixedSize(150, 40);
        button->setStyleSheet("background-color: lightgray;");

        int row = (i - 1) / 3;
        int col = (i - 1) % 3;

        if (i == 0) {
            row = 3;
            col = 1;
        }
        numPadLayout->addWidget(button, row, col);

        // Gérer l'appui sur un bouton numérique
        connect(button, &QPushButton::pressed, this, [this, button, i]() {
            button->setStyleSheet("background-color: gray;");

            if (passwordLineEdit->hasFocus()) {
                passwordLineEdit->setText(passwordLineEdit->text() + QString::number(i));
            } else {
                passwordLineEdit->setFocus();
                passwordLineEdit->setText(passwordLineEdit->text() + QString::number(i));
            }

            QTimer::singleShot(200, [button]() {
                button->setStyleSheet("background-color: lightgray;");
            });
        });
    }

    // Mise en page
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(idLabel);
    mainLayout->addWidget(idLineEdit);
    mainLayout->addWidget(passwordLabel);
    mainLayout->addWidget(passwordLineEdit);
    mainLayout->addWidget(passwordCheckBox);
    mainLayout->addWidget(loginButton, 0, Qt::AlignCenter);
    mainLayout->addWidget(messageLabel);
    mainLayout->addLayout(numPadLayout);

    setLayout(mainLayout);

    // Connexions
    connect(passwordCheckBox, &QCheckBox::toggled, this, &connexion1::togglePasswordVisibility);
    connect(idLineEdit, &QLineEdit::textChanged, this, &connexion1::checkFields);
    connect(passwordLineEdit, &QLineEdit::textChanged, this, &connexion1::checkFields);
    connect(loginButton, &QPushButton::clicked, this, &connexion1::authenticate);
}

bool connexion1::connectToDatabase()
{
    db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("localhost");
    db.setDatabaseName("Datacenter_v1");
    db.setUserName("anous");
    db.setPassword("Anousa@2006");

    if (!db.open()) {
        qDebug() << "Erreur : Impossible de se connecter à la base de données.";
        qDebug() << "Détails de l'erreur :" << db.lastError().text();
        return false;
    }
    qDebug() << "Connexion réussie à la base de données.";
    return true;
}

void connexion1::togglePasswordVisibility()
{
    if (passwordCheckBox->isChecked()) {
        passwordLineEdit->setEchoMode(QLineEdit::Normal);
    } else {
        passwordLineEdit->setEchoMode(QLineEdit::Password);
    }
}

void connexion1::checkFields()
{
    loginButton->setEnabled(!idLineEdit->text().isEmpty() && !passwordLineEdit->text().isEmpty());
}

void connexion1::authenticate()
{
    QString identifiant = idLineEdit->text();
    QString code = passwordLineEdit->text();

    loginButton->setStyleSheet("");

    QSqlQuery query;
    query.prepare("SELECT * FROM utilisateurs WHERE code = :code AND id = (SELECT id FROM utilisateurs WHERE identifiant = :identifiant)");
    query.bindValue(":code", code);
    query.bindValue(":identifiant", identifiant);

    if (query.exec() && query.next()) {
        loginButton->setStyleSheet("background-color: green;");
        messageLabel->setText("Authentification réussie.");
        messageLabel->setAlignment(Qt::AlignCenter);
        messageLabel->setStyleSheet("color: green;");
        errorCount = 0;
        QTimer::singleShot(500, this, [this](){
        this->close();
        menu = new Menu();
        menu->show();});
    }else {
        errorCount++;
        loginButton->setStyleSheet("background-color: red;");
        messageLabel->setText("Authentification échouée.");
        messageLabel->setAlignment(Qt::AlignCenter);
        messageLabel->setStyleSheet("color: red;");

        int attemptsLeft = 3 - errorCount;
        if (attemptsLeft > 0) {
            QMessageBox::warning(this, "Tentative incorrecte",
                                 QString("Identifiant ou mot de passe incorrect.\nIl vous reste %1 tentative(s).").arg(attemptsLeft));
        } else {
            QMessageBox::critical(this, "Échec", "Trop de tentatives échouées. L'application va se fermer.");
            close();
        }
    }
}

void connexion1::resizeEvent(QResizeEvent* evt){
    QWidget::resizeEvent(evt);
    this->set_bkgnd();
}

void connexion1::set_bkgnd(){
    QPixmap bkgnd("/home/anous/Images/DC.jpg");
    bkgnd = bkgnd.scaled(this->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    QPalette palette;
    palette.setBrush(QPalette::Background, bkgnd);
    this->setPalette(palette);
}
