
#ifndef ZONE0_H
#define ZONE0_H

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

class Zone0 : public QWidget
{
    Q_OBJECT

public:
    explicit Zone0(QWidget *parent = nullptr);

private slots:
    //void stopAllTimers();
    void showHistoriqueAlertes();
    void showHistoriqueAcces();
    void showListeEmployes();
    void retour();
    void updateHistoriqueAlertes();
    void updateHistoriqueAcces();
    void updateListeEmployes();    // Fonction pour mettre à jour automatiquement la table

private:
    QLabel *infoLabel;
    QTableWidget *tableWidget;
    Menu *menu;
    QTimer *refreshAccesTimer;  // Timer pour rafraîchir les données
    QTimer *refreshAlertesTimer;  // Timer pour rafraîchir les données

    void setupDatabase();
};

#endif // ZONE0_H



