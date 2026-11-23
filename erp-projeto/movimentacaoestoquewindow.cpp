#include "movimentacaoestoquewindow.h"
#include "ui_movimentacaoestoquewindow.h"
#include "loghelper.h"
#include "telashelper.h"
#include <QtSql>
#include <QDate>
#include <QMessageBox>

MovimentacaoEstoqueWindow::MovimentacaoEstoqueWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MovimentacaoEstoqueWindow)
{
    ui->setupUi(this);

    ui->spinBox->setMaximum(999999);            //quantidade (o padrao do Qt para em 99)
    ui->dateEdit->setDate(QDate::currentDate()); //data de hoje
    ui->lineEdit_3->setText(sessao().nome);      //responsavel = usuario logado

    carregarHistorico();
}

MovimentacaoEstoqueWindow::~MovimentacaoEstoqueWindow()
{
    delete ui;
}

//preenche a tabela com o historico de movimentacoes
void MovimentacaoEstoqueWindow::carregarHistorico()
{
    QSqlQuery q;
    if (!q.exec("SELECT m.data, COALESCE(p.descricao,'(produto removido)'), m.tipo, "
                "m.quantidade, COALESCE(m.usuario,'-') "
                "FROM tb_movimentacao_estoque m "
                "LEFT JOIN tb_produtos p ON p.id_produtos = m.id_produto "
                "ORDER BY m.id_mov DESC LIMIT 200")) {
        QMessageBox::warning(this, "Movimentação de Estoque",
                             "Erro ao consultar o histórico:\n" + q.lastError().text());
        return;
    }

    ui->tableWidget->setRowCount(0);
    while (q.next()) {
        int linha = ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(linha);
        for (int c = 0; c < 5; ++c)
            ui->tableWidget->setItem(linha, c,
                                     new QTableWidgetItem(q.value(c).toString()));
    }
}

//registra a movimentacao e atualiza o estoque do produto
void MovimentacaoEstoqueWindow::on_pushButton_clicked()
{
    QString produtoTexto = ui->lineEdit->text().trimmed();
    if (produtoTexto.isEmpty()) {
        QMessageBox::warning(this, "Movimentação de Estoque", "Informe o produto.");
        return;
    }

    //acha o produto pelo nome/descricao (ou codigo de barras)
    QSqlQuery busca;
    busca.prepare("SELECT id_produtos FROM tb_produtos "
                  "WHERE descricao LIKE ? OR codigo_barras = ? "
                  "ORDER BY descricao LIMIT 1");
    busca.addBindValue("%" + produtoTexto + "%");
    busca.addBindValue(produtoTexto);
    if (!busca.exec() || !busca.next()) {
        QMessageBox::information(this, "Movimentação de Estoque",
                                 "Produto não encontrado:\n" + produtoTexto);
        return;
    }
    int idProduto = busca.value(0).toInt();

    QString tipo = ui->comboBox->currentText();   //entrada / saida / ajuste / perda / devolucao
    int quantidade = ui->spinBox->value();
    if (quantidade <= 0) {
        QMessageBox::warning(this, "Movimentação de Estoque", "Informe a quantidade.");
        return;
    }

    //quantidade atual em estoque
    int atual = 0;
    QSqlQuery est;
    est.prepare("SELECT quantidade FROM tb_estoque WHERE id_produto=?");
    est.addBindValue(idProduto);
    if (est.exec() && est.next())
        atual = est.value(0).toInt();

    //calcula a nova quantidade conforme o tipo de movimentacao
    int novo = atual;
    if (tipo == "entrada" || tipo == "devolução")
        novo = atual + quantidade;
    else if (tipo == "saída" || tipo == "perda")
        novo = atual - quantidade;
    else if (tipo == "ajuste")
        novo = quantidade;   //no ajuste a quantidade digitada vira o novo saldo

    if (novo < 0) {
        QMessageBox::warning(this, "Movimentação de Estoque",
                             "Estoque insuficiente.\nSaldo atual: " +
                             QString::number(atual));
        return;
    }

    //atualiza (ou cria) a linha de estoque do produto
    QSqlQuery up;
    up.prepare("UPDATE tb_estoque SET quantidade=? WHERE id_produto=?");
    up.addBindValue(novo);
    up.addBindValue(idProduto);
    if (!up.exec() || up.numRowsAffected() == 0) {
        QSqlQuery ins;
        ins.prepare("INSERT INTO tb_estoque (id_produto, quantidade, localizacao) VALUES (?,?,'')");
        ins.addBindValue(idProduto);
        ins.addBindValue(novo);
        ins.exec();
    }

    //grava no historico
    QSqlQuery mov;
    mov.prepare("INSERT INTO tb_movimentacao_estoque (id_produto, tipo, quantidade, data, "
                "usuario, observacao) VALUES (?,?,?,?,?,?)");
    mov.addBindValue(idProduto);
    mov.addBindValue(tipo);
    mov.addBindValue(quantidade);
    mov.addBindValue(ui->dateEdit->date().toString("dd/MM/yyyy"));
    mov.addBindValue(ui->lineEdit_3->text().trimmed().isEmpty()
                     ? QString("-") : ui->lineEdit_3->text().trimmed());
    mov.addBindValue(ui->lineEdit_2->text().trimmed());
    if (!mov.exec()) {
        QMessageBox::warning(this, "Movimentação de Estoque",
                             "Erro ao registrar:\n" + mov.lastError().text());
        return;
    }

    registrarLog("Movimentação de estoque", "Estoque",
                 tipo + " de " + QString::number(quantidade) + " (saldo: " +
                 QString::number(novo) + ")");
    carregarHistorico();
    QMessageBox::information(this, "Movimentação de Estoque",
                             "Movimentação registrada com sucesso.\nNovo saldo: " +
                             QString::number(novo));
}

//limpa os campos para a proxima movimentacao
void MovimentacaoEstoqueWindow::on_pushButton_2_clicked()
{
    ui->lineEdit->clear();      //produto
    ui->lineEdit_2->clear();    //observacao
    ui->spinBox->setValue(0);   //quantidade
    ui->comboBox->setCurrentIndex(0);
    ui->dateEdit->setDate(QDate::currentDate());
    ui->lineEdit_3->setText(sessao().nome);
    ui->lineEdit->setFocus();
}
