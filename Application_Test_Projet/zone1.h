

#ifndef ZONE1_H
#define ZONE1_H

#include "menu.h"
#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QTableWidget>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QTimer>

class Zone1 : public QWidget
{
    Q_OBJECT

public:
    explicit Zone1(QWidget *parent = nullptr);
    ~Zone1();

private slots:
    void showHistoriqueAlertes();
    void showHistoriqueAcces();
    void showListeEmployes();
    void showEmployesBloques();  // Nouveau bouton pour afficher les employés bloqués
    void retour();
    void updateHistoriqueAlertes();
    void updateHistoriqueAcces();
    void updateListeEmployes();

    // Slot pour débloquer un employé depuis le bouton associé
    void debloquerEmployeFromButton();

private:
    QLabel *infoLabel;
    QTableWidget *tableWidget;
    Menu *menu;
    QTimer *refreshAccesTimer;
    QTimer *refreshAlertesTimer;

    void setupDatabase();
};

#endif // ZONE1_H
