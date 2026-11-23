#include "funcionariowindow.h"
#include "ui_funcionariowindow.h"
#include "loghelper.h"
#include <QtSql>
#include <QDate>
#include <QLineEdit>
#include <QMessageBox>

FuncionarioWindow::FuncionarioWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FuncionarioWindow)
{
    ui->setupUi(this);
}

FuncionarioWindow::~FuncionarioWindow()
{
    delete ui;
}

//limpa todos os campos da tela
void FuncionarioWindow::limparCampos()
{
    for (QLineEdit *campo : findChildren<QLineEdit *>())
        campo->clear();
    idAtual = 0;
}

//pesquisa pelo nome e carrega o funcionario na tela
void FuncionarioWindow::on_btn_pesquisar_clicked()
{
    QString nome = ui->lineEdit->text().trimmed();
    if (nome.isEmpty()) {
        QMessageBox::warning(this, "Funcionários", "Digite o nome para pesquisar.");
        return;
    }

    QSqlQuery q;
    q.prepare("SELECT * FROM tb_funcionarios WHERE nome LIKE ? ORDER BY nome LIMIT 1");
    q.addBindValue("%" + nome + "%");
    if (!q.exec() || !q.next()) {
        QMessageBox::information(this, "Funcionários", "Funcionário não encontrado.");
        idAtual = 0;
        return;
    }

    idAtual = q.value("id_funcionario").toInt();
    ui->lineEdit->setText(q.value("nome").toString());           //Nome
    ui->lineEdit_2->setText(q.value("cpf").toString());          //CPF
    ui->lineEdit_3->setText(q.value("cargo").toString());        //Cargo
    ui->lineEdit_4->setText(q.value("telefone").toString());     //Telefone
    ui->lineEdit_5->setText(q.value("email").toString());        //Email
    ui->lineEdit_6->setText(QString::number(q.value("salario").toDouble(), 'f', 2)); //Salario
    ui->lineEdit_7->setText(q.value("data_adimissao").toString()); //Data admissao
    ui->lineEdit_8->setText(q.value("ativo").toInt() == 1 ? "ATIVO" : "INATIVO");    //Status
}

//insere um novo funcionario
void FuncionarioWindow::on_btn_inserir_clicked()
{
    QString nome = ui->lineEdit->text().trimmed();
    if (nome.isEmpty()) {
        QMessageBox::warning(this, "Funcionários", "Informe o nome do funcionário.");
        return;
    }

    QSqlQuery q;
    q.prepare("INSERT INTO tb_funcionarios (nome, cpf, cargo, telefone, email, "
              "salario, data_adimissao, ativo) VALUES (?,?,?,?,?,?,?,?)");
    q.addBindValue(nome);
    q.addBindValue(ui->lineEdit_2->text().trimmed());   //CPF
    q.addBindValue(ui->lineEdit_3->text().trimmed());   //Cargo
    q.addBindValue(ui->lineEdit_4->text().trimmed());   //Telefone
    q.addBindValue(ui->lineEdit_5->text().trimmed());   //Email
    q.addBindValue(ui->lineEdit_6->text().replace(',', '.').toDouble()); //Salario
    q.addBindValue(ui->lineEdit_7->text().trimmed());   //Data admissao
    //status: so "INATIVO" (ou "0") desativa
    q.addBindValue(ui->lineEdit_8->text().trimmed().toUpper().startsWith("INA") ? 0 : 1);

    if (!q.exec()) {
        QMessageBox::warning(this, "Funcionários", "Erro ao inserir:\n" + q.lastError().text());
        return;
    }
    idAtual = q.lastInsertId().toInt();
    registrarLog("Inserção", "Funcionários", "nome: " + nome);
    QMessageBox::information(this, "Funcionários", "Funcionário inserido com sucesso.");
}

//altera o funcionario carregado na tela
void FuncionarioWindow::on_btn_alterar_clicked()
{
    if (idAtual == 0) {
        QMessageBox::warning(this, "Funcionários", "Pesquise um funcionário antes de alterar.");
        return;
    }
    QString nome = ui->lineEdit->text().trimmed();
    if (nome.isEmpty()) {
        QMessageBox::warning(this, "Funcionários", "Informe o nome do funcionário.");
        return;
    }

    QSqlQuery q;
    q.prepare("UPDATE tb_funcionarios SET nome=?, cpf=?, cargo=?, telefone=?, email=?, "
              "salario=?, data_adimissao=?, ativo=? WHERE id_funcionario=?");
    q.addBindValue(nome);
    q.addBindValue(ui->lineEdit_2->text().trimmed());   //CPF
    q.addBindValue(ui->lineEdit_3->text().trimmed());   //Cargo
    q.addBindValue(ui->lineEdit_4->text().trimmed());   //Telefone
    q.addBindValue(ui->lineEdit_5->text().trimmed());   //Email
    q.addBindValue(ui->lineEdit_6->text().replace(',', '.').toDouble()); //Salario
    q.addBindValue(ui->lineEdit_7->text().trimmed());   //Data admissao
    q.addBindValue(ui->lineEdit_8->text().trimmed().toUpper().startsWith("INA") ? 0 : 1);
    q.addBindValue(idAtual);

    if (!q.exec()) {
        QMessageBox::warning(this, "Funcionários", "Erro ao alterar:\n" + q.lastError().text());
        return;
    }
    registrarLog("Alteração", "Funcionários", "nome: " + nome);
    QMessageBox::information(this, "Funcionários", "Funcionário alterado com sucesso.");
}

//exclui o funcionario carregado na tela
void FuncionarioWindow::on_btn_excluir_clicked()
{
    if (idAtual == 0) {
        QMessageBox::warning(this, "Funcionários", "Pesquise um funcionário antes de excluir.");
        return;
    }
    if (QMessageBox::question(this, "Funcionários", "Excluir este funcionário?") !=
        QMessageBox::Yes)
        return;

    QSqlQuery q;
    q.prepare("DELETE FROM tb_funcionarios WHERE id_funcionario=?");
    q.addBindValue(idAtual);
    if (!q.exec()) {
        QMessageBox::warning(this, "Funcionários", "Erro ao excluir:\n" + q.lastError().text());
        return;
    }
    registrarLog("Exclusão", "Funcionários", "id: " + QString::number(idAtual));
    limparCampos();
    QMessageBox::information(this, "Funcionários", "Funcionário excluído com sucesso.");
}
