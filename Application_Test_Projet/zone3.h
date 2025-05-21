#ifndef ZONE3_H
#define ZONE3_H

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
class Zone3 : public QWidget
{
    Q_OBJECT

public:
    explicit Zone3(QWidget *parent = nullptr);
    ~Zone3();


private slots:
    void showHistoriqueAlertes();
    void showHistoriqueAcces();
    void showListeEmployes();
    void showEmployesBloques();
    void retour();
    void updateHistoriqueAlertes();
    void updateHistoriqueAcces();
    void updateListeEmployes();
    void debloquerEmployeFromButton();

private:
    QLabel *infoLabel;
    QTableWidget *tableWidget;
    Menu *menu;
    QTimer *refreshAccesTimer;  // Timer pour rafraîchir les données
    QTimer *refreshAlertesTimer;
    void setupDatabase();
};

#endif // ZONE3_H
