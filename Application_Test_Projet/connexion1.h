
#ifndef CONNEXION1_H
#define CONNEXION1_H

#include "menu.h"
#include <QWidget>
#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QTimer>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>

class Menu;

class connexion1 : public QWidget
{
    Q_OBJECT

public:
    explicit connexion1(QWidget *parent = nullptr);

    bool connectToDatabase();
    void set_bkgnd();

signals:
    void authenticated();

private slots:
    void togglePasswordVisibility();
    void checkFields();
    void authenticate();

protected:
    void resizeEvent(QResizeEvent* evt) override;

private:
    QLineEdit *idLineEdit;
    QLineEdit *passwordLineEdit;
    QCheckBox *passwordCheckBox;
    QCheckBox *passwordCheckBox1;
    QPushButton *loginButton;
    QLabel *messageLabel;
    QSqlDatabase db;
    Menu *menu;
    int errorCount = 0; // Compteur d'erreurs
};

#endif // CONNEXION1_H



