#include "fornecedorwindow.h"
#include "ui_fornecedorwindow.h"
#include "loghelper.h"
#include <QtSql>
#include <QLineEdit>
#include <QMessageBox>

FornecedorWindow::FornecedorWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FornecedorWindow)
{
    ui->setupUi(this);
}

FornecedorWindow::~FornecedorWindow()
{
    delete ui;
}

//limpa todos os campos da tela
void FornecedorWindow::limparCampos()
{
    for (QLineEdit *campo : findChildren<QLineEdit *>())
        campo->clear();
    idAtual = 0;
}

//pesquisa pelo nome e carrega o fornecedor na tela
void FornecedorWindow::on_pushButton_6_clicked()
{
    QString nome = ui->lineEdit_14->text().trimmed();
    if (nome.isEmpty()) {
        QMessageBox::warning(this, "Fornecedores", "Digite o nome para pesquisar.");
        return;
    }

    QSqlQuery q;
    q.prepare("SELECT * FROM tb_fornecedores WHERE nome LIKE ? ORDER BY nome LIMIT 1");
    q.addBindValue("%" + nome + "%");
    if (!q.exec() || !q.next()) {
        QMessageBox::information(this, "Fornecedores", "Fornecedor não encontrado.");
        idAtual = 0;
        return;
    }

    idAtual = q.value("id_fornecedor").toInt();
    ui->lineEdit_14->setText(q.value("nome").toString());     //Nome
    ui->lineEdit_15->setText(q.value("cnpj").toString());     //CPF/CNPJ
    ui->lineEdit_16->setText(q.value("endereco").toString()); //Endereco
    ui->lineEdit_17->setText(q.value("cep").toString());      //Cep
    ui->lineEdit_18->setText(q.value("cidade").toString());   //Cidade
    ui->lineEdit_19->setText(q.value("telefone").toString()); //Telefone
    ui->lineEdit_20->setText(q.value("bairro").toString());   //Bairro
    ui->lineEdit_21->setText(q.value("estado").toString());   //Estado
    ui->lineEdit_22->setText(q.value("celular").toString());  //Celular
    ui->lineEdit_23->setText(q.value("email").toString());    //Email
}

//insere um novo fornecedor
void FornecedorWindow::on_pushButton_7_clicked()
{
    QString nome = ui->lineEdit_14->text().trimmed();
    if (nome.isEmpty()) {
        QMessageBox::warning(this, "Fornecedores", "Informe o nome do fornecedor.");
        return;
    }

    QSqlQuery q;
    q.prepare("INSERT INTO tb_fornecedores (nome, cnpj, telefone, celular, email, "
              "endereco, bairro, cidade, estado, cep) VALUES (?,?,?,?,?,?,?,?,?,?)");
    q.addBindValue(nome);
    q.addBindValue(ui->lineEdit_15->text().trimmed());  //CNPJ
    q.addBindValue(ui->lineEdit_19->text().trimmed());  //Telefone
    q.addBindValue(ui->lineEdit_22->text().trimmed());  //Celular
    q.addBindValue(ui->lineEdit_23->text().trimmed());  //Email
    q.addBindValue(ui->lineEdit_16->text().trimmed());  //Endereco
    q.addBindValue(ui->lineEdit_20->text().trimmed());  //Bairro
    q.addBindValue(ui->lineEdit_18->text().trimmed());  //Cidade
    q.addBindValue(ui->lineEdit_21->text().trimmed());  //Estado
    q.addBindValue(ui->lineEdit_17->text().trimmed());  //Cep

    if (!q.exec()) {
        QMessageBox::warning(this, "Fornecedores", "Erro ao inserir:\n" + q.lastError().text());
        return;
    }
    idAtual = q.lastInsertId().toInt();
    registrarLog("Inserção", "Fornecedores", "nome: " + nome);
    QMessageBox::information(this, "Fornecedores", "Fornecedor inserido com sucesso.");
}

//altera o fornecedor carregado na tela
void FornecedorWindow::on_pushButton_8_clicked()
{
    if (idAtual == 0) {
        QMessageBox::warning(this, "Fornecedores", "Pesquise um fornecedor antes de alterar.");
        return;
    }
    QString nome = ui->lineEdit_14->text().trimmed();
    if (nome.isEmpty()) {
        QMessageBox::warning(this, "Fornecedores", "Informe o nome do fornecedor.");
        return;
    }

    QSqlQuery q;
    q.prepare("UPDATE tb_fornecedores SET nome=?, cnpj=?, telefone=?, celular=?, "
              "email=?, endereco=?, bairro=?, cidade=?, estado=?, cep=? "
              "WHERE id_fornecedor=?");
    q.addBindValue(nome);
    q.addBindValue(ui->lineEdit_15->text().trimmed());  //CNPJ
    q.addBindValue(ui->lineEdit_19->text().trimmed());  //Telefone
    q.addBindValue(ui->lineEdit_22->text().trimmed());  //Celular
    q.addBindValue(ui->lineEdit_23->text().trimmed());  //Email
    q.addBindValue(ui->lineEdit_16->text().trimmed());  //Endereco
    q.addBindValue(ui->lineEdit_20->text().trimmed());  //Bairro
    q.addBindValue(ui->lineEdit_18->text().trimmed());  //Cidade
    q.addBindValue(ui->lineEdit_21->text().trimmed());  //Estado
    q.addBindValue(ui->lineEdit_17->text().trimmed());  //Cep
    q.addBindValue(idAtual);

    if (!q.exec()) {
        QMessageBox::warning(this, "Fornecedores", "Erro ao alterar:\n" + q.lastError().text());
        return;
    }
    registrarLog("Alteração", "Fornecedores", "nome: " + nome);
    QMessageBox::information(this, "Fornecedores", "Fornecedor alterado com sucesso.");
}

//exclui o fornecedor carregado na tela
void FornecedorWindow::on_pushButton_9_clicked()
{
    if (idAtual == 0) {
        QMessageBox::warning(this, "Fornecedores", "Pesquise um fornecedor antes de excluir.");
        return;
    }
    if (QMessageBox::question(this, "Fornecedores", "Excluir este fornecedor?") !=
        QMessageBox::Yes)
        return;

    QSqlQuery q;
    q.prepare("DELETE FROM tb_fornecedores WHERE id_fornecedor=?");
    q.addBindValue(idAtual);
    if (!q.exec()) {
        QMessageBox::warning(this, "Fornecedores", "Erro ao excluir:\n" + q.lastError().text());
        return;
    }
    registrarLog("Exclusão", "Fornecedores", "id: " + QString::number(idAtual));
    limparCampos();
    QMessageBox::information(this, "Fornecedores", "Fornecedor excluído com sucesso.");
}
