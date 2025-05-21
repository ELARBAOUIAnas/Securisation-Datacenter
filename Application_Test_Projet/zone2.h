#ifndef ZONE2_H
#define ZONE2_H

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
class Zone2 : public QWidget
{
    Q_OBJECT

public:
    explicit Zone2(QWidget *parent = nullptr);

private slots:
    void showHistoriqueAlertes();
    void showHistoriqueAcces();
    void showListeEmployes();
    void retour();
    void updateHistoriqueAlertes();
    void updateHistoriqueAcces();
    void updateListeEmployes();

private:
    QLabel *infoLabel;
    QTableWidget *tableWidget;
    Menu *menu;
    QTimer *refreshAccesTimer;  // Timer pour rafraîchir les données
    QTimer *refreshAlertesTimer;
    void setupDatabase();
};

#endif // ZONE2_H
