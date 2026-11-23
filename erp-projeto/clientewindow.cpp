#include "clientewindow.h"
#include "ui_clientewindow.h"
#include "loghelper.h"
#include <QtSql>
#include <QDate>
#include <QLineEdit>
#include <QMessageBox>

ClienteWindow::ClienteWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ClienteWindow)
{
    ui->setupUi(this);
}

ClienteWindow::~ClienteWindow()
{
    delete ui;
}

//limpa todos os campos da tela
void ClienteWindow::limparCampos()
{
    for (QLineEdit *campo : findChildren<QLineEdit *>())
        campo->clear();
    idAtual = 0;
}

//pesquisa pelo nome e carrega o cliente na tela
void ClienteWindow::on_pushButton_4_clicked()
{
    QString nome = ui->lineEdit->text().trimmed();
    if (nome.isEmpty()) {
        QMessageBox::warning(this, "Clientes", "Digite o nome para pesquisar.");
        return;
    }

    QSqlQuery q;
    q.prepare("SELECT * FROM tb_clientes WHERE nome LIKE ? ORDER BY nome LIMIT 1");
    q.addBindValue("%" + nome + "%");
    if (!q.exec() || !q.next()) {
        QMessageBox::information(this, "Clientes", "Cliente não encontrado.");
        idAtual = 0;
        return;
    }

    idAtual = q.value("id_cliente").toInt();
    ui->lineEdit->setText(q.value("nome").toString());        //Nome
    ui->lineEdit_2->setText(q.value("cpf_cnpj").toString());  //CPF
    ui->lineEdit_5->setText(q.value("endereco").toString());  //Endereco
    ui->lineEdit_3->setText(q.value("cep").toString());       //Cep
    ui->lineEdit_7->setText(q.value("cidade").toString());    //Cidade
    ui->lineEdit_4->setText(q.value("telefone").toString());  //Telefone
    ui->lineEdit_6->setText(q.value("bairro").toString());    //Bairro
    ui->lineEdit_8->setText(q.value("estado").toString());    //Estado
    ui->lineEdit_10->setText(q.value("celular").toString());  //Celular
    ui->lineEdit_9->setText(q.value("email").toString());     //Email
}

//insere um novo cliente
void ClienteWindow::on_pushButton_clicked()
{
    QString nome = ui->lineEdit->text().trimmed();
    if (nome.isEmpty()) {
        QMessageBox::warning(this, "Clientes", "Informe o nome do cliente.");
        return;
    }

    QSqlQuery q;
    q.prepare("INSERT INTO tb_clientes (nome, cpf_cnpj, telefone, celular, email, "
              "endereco, bairro, cidade, estado, cep, data_cadastro, ativo) "
              "VALUES (?,?,?,?,?,?,?,?,?,?,?,1)");
    q.addBindValue(nome);
    q.addBindValue(ui->lineEdit_2->text().trimmed());   //CPF
    q.addBindValue(ui->lineEdit_4->text().trimmed());   //Telefone
    q.addBindValue(ui->lineEdit_10->text().trimmed());  //Celular
    q.addBindValue(ui->lineEdit_9->text().trimmed());   //Email
    q.addBindValue(ui->lineEdit_5->text().trimmed());   //Endereco
    q.addBindValue(ui->lineEdit_6->text().trimmed());   //Bairro
    q.addBindValue(ui->lineEdit_7->text().trimmed());   //Cidade
    q.addBindValue(ui->lineEdit_8->text().trimmed());   //Estado
    q.addBindValue(ui->lineEdit_3->text().trimmed());   //Cep
    q.addBindValue(QDate::currentDate().toString("dd/MM/yyyy"));

    if (!q.exec()) {
        QMessageBox::warning(this, "Clientes", "Erro ao inserir:\n" + q.lastError().text());
        return;
    }
    idAtual = q.lastInsertId().toInt();
    registrarLog("Inserção", "Clientes", "nome: " + nome);
    QMessageBox::information(this, "Clientes", "Cliente inserido com sucesso.");
}

//altera o cliente carregado na tela
void ClienteWindow::on_pushButton_2_clicked()
{
    if (idAtual == 0) {
        QMessageBox::warning(this, "Clientes", "Pesquise um cliente antes de alterar.");
        return;
    }
    QString nome = ui->lineEdit->text().trimmed();
    if (nome.isEmpty()) {
        QMessageBox::warning(this, "Clientes", "Informe o nome do cliente.");
        return;
    }

    QSqlQuery q;
    q.prepare("UPDATE tb_clientes SET nome=?, cpf_cnpj=?, telefone=?, celular=?, "
              "email=?, endereco=?, bairro=?, cidade=?, estado=?, cep=? "
              "WHERE id_cliente=?");
    q.addBindValue(nome);
    q.addBindValue(ui->lineEdit_2->text().trimmed());   //CPF
    q.addBindValue(ui->lineEdit_4->text().trimmed());   //Telefone
    q.addBindValue(ui->lineEdit_10->text().trimmed());  //Celular
    q.addBindValue(ui->lineEdit_9->text().trimmed());   //Email
    q.addBindValue(ui->lineEdit_5->text().trimmed());   //Endereco
    q.addBindValue(ui->lineEdit_6->text().trimmed());   //Bairro
    q.addBindValue(ui->lineEdit_7->text().trimmed());   //Cidade
    q.addBindValue(ui->lineEdit_8->text().trimmed());   //Estado
    q.addBindValue(ui->lineEdit_3->text().trimmed());   //Cep
    q.addBindValue(idAtual);

    if (!q.exec()) {
        QMessageBox::warning(this, "Clientes", "Erro ao alterar:\n" + q.lastError().text());
        return;
    }
    registrarLog("Alteração", "Clientes", "nome: " + nome);
    QMessageBox::information(this, "Clientes", "Cliente alterado com sucesso.");
}

//exclui o cliente carregado na tela
void ClienteWindow::on_pushButton_3_clicked()
{
    if (idAtual == 0) {
        QMessageBox::warning(this, "Clientes", "Pesquise um cliente antes de excluir.");
        return;
    }
    if (QMessageBox::question(this, "Clientes", "Excluir este cliente?") !=
        QMessageBox::Yes)
        return;

    QSqlQuery q;
    q.prepare("DELETE FROM tb_clientes WHERE id_cliente=?");
    q.addBindValue(idAtual);
    if (!q.exec()) {
        QMessageBox::warning(this, "Clientes", "Erro ao excluir:\n" + q.lastError().text());
        return;
    }
    registrarLog("Exclusão", "Clientes", "id: " + QString::number(idAtual));
    limparCampos();
    QMessageBox::information(this, "Clientes", "Cliente excluído com sucesso.");
}
