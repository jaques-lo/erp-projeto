#include "loginwindow.h"
#include "ui_loginwindow.h"
#include "mainwindow.h"
#include "banco.h"
#include "loghelper.h"
#include "telashelper.h"
#include <QMessageBox>
#include <QApplication>
#include <QDateTime>
#include <QDir>

LoginWindow::LoginWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LoginWindow)
{
    ui->setupUi(this);
    //ela da o caminho relativo do banco de dados, acha onde esta o .exe do projeto
    QString end_banco = qApp->applicationDirPath() + "/db_erp";

    bancoDeDados.setDatabaseName(end_banco);

    if (!bancoDeDados.open()) {
        QMessageBox::warning(this, "Banco de Dados", "Banco de dados não abriu");
        return;
    }

    //garante que todas as tabelas do ERP existem (banco novo, antigo ou vazio)
    criarTabelasBanco(bancoDeDados);
}

LoginWindow::~LoginWindow()
{
    delete ui;
}

void LoginWindow::on_btn_login_clicked()
{
    QString user = ui->edt_id->text().trimmed();
    QString code = ui->edt_senha->text();

    if(!bancoDeDados.open()){
        QMessageBox::warning(this, "Banco de Dados", "Banco de dados não abriu");
        return;
    }

    QSqlQuery query(bancoDeDados);
    //aceita o codigo (id) ou o login do usuario
    if(query.exec("SELECT * FROM tb_usuarios WHERE (id_usuario='" + user +
                  "' OR login='" + user + "') AND senha='" + code +
                  "' AND ativo=1")){
        if(query.next()){
            //guarda quem esta logado (usado nos logs e nas movimentacoes)
            sessao().id   = query.value("id_usuario").toInt();
            sessao().nome = query.value("nome").toString();
            sessao().nivel= query.value("nivel").toInt();

            //atualiza o ultimo login
            QSqlQuery up(bancoDeDados);
            up.prepare("UPDATE tb_usuarios SET ultimo_login=? WHERE id_usuario=?");
            up.addBindValue(QDateTime::currentDateTime().toString("dd/MM/yyyy HH:mm:ss"));
            up.addBindValue(sessao().id);
            up.exec();

            //auditoria
            registrarLog("Login", "Login", "usuario: " + user);

            //login certo: abre a tela principal do sistema e fecha o login
            MainWindow *principal = new MainWindow();
            principal->show();
            close();
        }else{
            QMessageBox::warning(this,"Login","Usuário ou senha incorretos");
            ui->edt_senha->clear();
            ui->edt_id->clear();
            ui->edt_id->setFocus();
        }
    }else{
        //a consulta falhou (ex.: banco corrompido)
        QMessageBox::warning(this,"Login",
                             "Falha ao consultar o banco de dados:\n" + query.lastError().text());
    }
}

void LoginWindow::on_btn_exit_clicked()
{
    qApp->quit();
}
