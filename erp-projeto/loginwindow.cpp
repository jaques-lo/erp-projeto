#include "loginwindow.h"
#include "ui_loginwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QDir>

LoginWindow::LoginWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LoginWindow)
{
    ui->setupUi(this);
    //ela da o caminho relativo do banco de dados, acha onde esta o .exe do projeto
    QString end_banco = qApp->applicationDirPath() + "/db_erp";
    //QString end_banco = "/home/usuario/Documentos/ERP/erp/erp-projeto/db_erp";

/*
    //=========serve para criar a pasta===============
    QDir dir;
    dir.mkpath(qApp->applicationDirPath() + "/db");
*/
    bancoDeDados.setDatabaseName(end_banco);

    if (!bancoDeDados.open()) {
        QMessageBox::warning(this, "Banco de Dados", "Banco de dados não abriu");
        return;
    }

}

LoginWindow::~LoginWindow()
{
    delete ui;
}

void LoginWindow::on_btn_login_clicked()
{
    QString user = ui->edt_id->text();
    QString code = ui->edt_senha->text();

    if(!bancoDeDados.open()){
        QMessageBox::warning(this, "Banco de Dados", "Banco de dados não abriu");
        return;
    }

    QSqlQuery query;
    QString id = ui->edt_id->text();
    if(query.exec("SELECT * FROM tb_usuarios WHERE id_usuario='" + id +
                  "' AND senha='" + code + "'")){
        int cont=0;
        while(query.next()){
            cont++;
        }
        if(cont>0){
           QMainWindow *principal = new QMainWindow();
            principal->show();
            close();
        }else{
        QMessageBox::warning(this,"Login","Usuário ou senha incorretos");
        ui->edt_senha->clear();
        ui->edt_id->clear();
        ui->edt_id->setFocus();
        }
    }
}

