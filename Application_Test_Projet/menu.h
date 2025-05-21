/*#ifndef MENU_H
#define MENU_H

#include "connexion1.h"
#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>

class Menu : public QWidget
{
    Q_OBJECT

public:
    explicit Menu(QWidget *parent = nullptr);

private slots:
    void zoneWin0();
    void zoneWin1();
    void zoneWin2();
    void zoneWin3();
    void deconnexion();  // Slot pour gérer la déconnexion
    void quitter();      // Slot pour gérer la fermeture de l'application

private:
    QPushButton *zone0Button;
    QPushButton *zone1Button;
    QPushButton *zone2Button;
    QPushButton *zone3Button;
    QPushButton *deconnexionButton;
    QLabel *infoLabel;
    QPushButton *quitterButton;
};

#endif // MENU_H
*/
// Dans menu.h
// menu.h
#ifndef MENU_H
#define MENU_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>

class Menu : public QMainWindow
{
    Q_OBJECT

public:
    explicit Menu(QWidget *parent = nullptr);

private slots:
    void zoneWin0();
    void zoneWin1();
    void zoneWin2();
    void zoneWin3();
    void deconnexion();
    void quitter();

private:
    QLabel *infoLabel;
};

#endif // MENU_H
