#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QWidget>

#include<QtSql>
#include<QDialog>

namespace Ui {
class LoginWindow;
}

class LoginWindow : public QWidget
{
    Q_OBJECT

public:
    explicit LoginWindow(QWidget *parent = nullptr);
    ~LoginWindow();

private slots:
    void on_btn_login_clicked();

private:
    Ui::LoginWindow *ui;
    QSqlDatabase bancoDeDados = QSqlDatabase::addDatabase("QSQLITE");
};

#endif // LOGINWINDOW_H
