#include "comprawindow.h"
#include "ui_comprawindow.h"
#include "loghelper.h"
#include "telashelper.h"
#include <QtSql>
#include <QDate>
#include <QInputDialog>
#include <QMessageBox>

CompraWindow::CompraWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CompraWindow)
{
    ui->setupUi(this);
    ui->doubleSpinBox->setMaximum(99999999.99);  //total (o padrao do Qt para em 99.99)
    ui->doubleSpinBox->setReadOnly(true);        //so exibe o total do carrinho
    ui->doubleSpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
}

CompraWindow::~CompraWindow()
{
    delete ui;
}

//soma os subtotais dos itens do carrinho
double CompraWindow::totalCarrinho() const
{
    double total = 0;
    for (int l = 0; l < ui->tableWidget->rowCount(); ++l)
        total += ui->tableWidget->item(l, 4)->text().toDouble();
    return total;
}

//limpa a tela para uma nova compra
void CompraWindow::limparTudo()
{
    ui->tableWidget->setRowCount(0);
    ui->lineEdit->clear();        //fornecedor
    ui->lineEdit_2->clear();      //nota fiscal
    ui->comboBox->setCurrentIndex(0);
    ui->doubleSpinBox->setValue(0);
    idFornecedor = 0;
}

//busca o fornecedor pelo nome
void CompraWindow::on_pushButton_5_clicked()
{
    QString nome = ui->lineEdit->text().trimmed();
    if (nome.isEmpty()) {
        QMessageBox::warning(this, "Compras", "Digite o nome do fornecedor para buscar.");
        return;
    }

    QSqlQuery q;
    q.prepare("SELECT id_fornecedor, nome FROM tb_fornecedores WHERE nome LIKE ? "
              "ORDER BY nome LIMIT 1");
    q.addBindValue("%" + nome + "%");
    if (!q.exec() || !q.next()) {
        QMessageBox::information(this, "Compras", "Fornecedor não encontrado.");
        idFornecedor = 0;
        return;
    }
    idFornecedor = q.value(0).toInt();
    ui->lineEdit->setText(q.value(1).toString());   //mostra o nome completo encontrado
    QMessageBox::information(this, "Compras", "Fornecedor selecionado:\n" +
                             q.value(1).toString());
}

//adiciona um item no carrinho (produto + quantidade)
void CompraWindow::on_pushButton_clicked()
{
    QString texto = QInputDialog::getText(this, "Adicionar Item",
                                          "Produto (descrição ou código de barras):");
    if (texto.trimmed().isEmpty())
        return;

    QSqlQuery q;
    q.prepare("SELECT id_produtos, COALESCE(NULLIF(codigo_barras,''), codigo_barras), "
              "descricao, custo, preco FROM tb_produtos "
              "WHERE descricao LIKE ? OR codigo_barras = ? ORDER BY descricao LIMIT 1");
    q.addBindValue("%" + texto.trimmed() + "%");
    q.addBindValue(texto.trimmed());
    if (!q.exec() || !q.next()) {
        QMessageBox::information(this, "Compras", "Produto não encontrado:\n" + texto);
        return;
    }

    int idProduto = q.value(0).toInt();
    QString codigo = q.value(1).toString().isEmpty()
                     ? QString::number(idProduto) : q.value(1).toString();
    QString descricao = q.value(2).toString();
    //compra entra pelo preco de CUSTO
    double custo = q.value(3).toDouble();
    if (custo <= 0)
        custo = q.value(4).toDouble();

    bool ok = false;
    int quantidade = QInputDialog::getInt(this, "Adicionar Item",
                                          "Quantidade de \"" + descricao + "\":",
                                          1, 1, 999999, 1, &ok);
    if (!ok || quantidade <= 0)
        return;

    double subtotal = custo * quantidade;
    int linha = ui->tableWidget->rowCount();
    ui->tableWidget->insertRow(linha);
    ui->tableWidget->setItem(linha, 0, new QTableWidgetItem(codigo));            //Codigo
    ui->tableWidget->setItem(linha, 1, new QTableWidgetItem(descricao));         //Produto
    ui->tableWidget->setItem(linha, 2, new QTableWidgetItem(QString::number(quantidade))); //Qtd
    ui->tableWidget->setItem(linha, 3, new QTableWidgetItem(QString::number(custo, 'f', 2))); //Custo
    ui->tableWidget->setItem(linha, 4, new QTableWidgetItem(QString::number(subtotal, 'f', 2))); //Subtotal
    ui->tableWidget->item(linha, 0)->setData(Qt::UserRole, idProduto);
    for (int c = 0; c < ui->tableWidget->columnCount(); ++c)
        ui->tableWidget->item(linha, c)->setFlags(ui->tableWidget->item(linha, c)->flags()
                                                  & ~Qt::ItemIsEditable);

    ui->doubleSpinBox->setValue(totalCarrinho());
}

//remove o item selecionado do carrinho
void CompraWindow::on_pushButton_2_clicked()
{
    int linha = ui->tableWidget->currentRow();
    if (linha < 0) {
        QMessageBox::warning(this, "Compras", "Selecione um item para remover.");
        return;
    }
    ui->tableWidget->removeRow(linha);
    ui->doubleSpinBox->setValue(totalCarrinho());
}

//finaliza a compra: grava compra + itens, da entrada no estoque e trata o pagamento
void CompraWindow::on_pushButton_3_clicked()
{
    if (idFornecedor == 0) {
        QMessageBox::warning(this, "Compras", "Busque o fornecedor antes de finalizar.");
        return;
    }
    if (ui->tableWidget->rowCount() == 0) {
        QMessageBox::warning(this, "Compras", "Adicione pelo menos um item.");
        return;
    }
    if (QMessageBox::question(this, "Compras", "Finalizar esta compra?") !=
        QMessageBox::Yes)
        return;

    double total = totalCarrinho();
    QString data = QDate::currentDate().toString("dd/MM/yyyy");
    QString forma = ui->comboBox->currentText();   //A vista / Prazo

    //1) cabecalho da compra
    QSqlQuery compra;
    compra.prepare("INSERT INTO tb_compras (id_fornecedor, id_usuario, data, valor_total, "
                   "status, nota_fiscal) VALUES (?,?,?,?,?,?)");
    compra.addBindValue(idFornecedor);
    compra.addBindValue(sessao().id);
    compra.addBindValue(data);
    compra.addBindValue(total);
    compra.addBindValue("finalizada");
    compra.addBindValue(ui->lineEdit_2->text().trimmed());   //nota fiscal
    if (!compra.exec()) {
        QMessageBox::warning(this, "Compras", "Erro ao gravar a compra:\n" +
                             compra.lastError().text());
        return;
    }
    int idCompra = compra.lastInsertId().toInt();

    //2) itens + entrada no estoque + movimentacao
    for (int l = 0; l < ui->tableWidget->rowCount(); ++l) {
        int idProduto = ui->tableWidget->item(l, 0)->data(Qt::UserRole).toInt();
        int quantidade = ui->tableWidget->item(l, 2)->text().toInt();
        double custo = ui->tableWidget->item(l, 3)->text().toDouble();
        double subtotal = ui->tableWidget->item(l, 4)->text().toDouble();

        QSqlQuery item;
        item.prepare("INSERT INTO tb_itens_compra (id_compra, id_produto, quantidade, "
                     "valor_unitario, subtotal) VALUES (?,?,?,?,?)");
        item.addBindValue(idCompra);
        item.addBindValue(idProduto);
        item.addBindValue(quantidade);
        item.addBindValue(custo);
        item.addBindValue(subtotal);
        item.exec();

        //entrada no estoque
        QSqlQuery est;
        est.prepare("UPDATE tb_estoque SET quantidade = quantidade + ? WHERE id_produto=?");
        est.addBindValue(quantidade);
        est.addBindValue(idProduto);
        if (!est.exec() || est.numRowsAffected() == 0) {
            QSqlQuery ins;
            ins.prepare("INSERT INTO tb_estoque (id_produto, quantidade, localizacao) VALUES (?,?,'')");
            ins.addBindValue(idProduto);
            ins.addBindValue(quantidade);
            ins.exec();
        }

        //historico de movimentacao
        QSqlQuery mov;
        mov.prepare("INSERT INTO tb_movimentacao_estoque (id_produto, tipo, quantidade, "
                    "data, usuario, observacao) VALUES (?,?,?,?,?,?)");
        mov.addBindValue(idProduto);
        mov.addBindValue("entrada");
        mov.addBindValue(quantidade);
        mov.addBindValue(data);
        mov.addBindValue(sessao().nome.isEmpty() ? QString("-") : sessao().nome);
        mov.addBindValue("compra #" + QString::number(idCompra));
        mov.exec();
    }

    //3) pagamento
    if (forma == "Prazo") {
        //gera a conta a pagar (vencimento em 30 dias)
        QSqlQuery pagar;
        pagar.prepare("INSERT INTO tb_pagar (id_fornecedor, categoria, valor, data_emissao, "
                      "data_vencimento, status, forma_pagamento) VALUES (?,?,?,?,?,?,?)");
        pagar.addBindValue(idFornecedor);
        pagar.addBindValue("Compra de mercadoria");
        pagar.addBindValue(total);
        pagar.addBindValue(data);
        pagar.addBindValue(QDate::currentDate().addDays(30).toString("dd/MM/yyyy"));
        pagar.addBindValue("pendente");
        pagar.addBindValue("Prazo");
        pagar.exec();
    } else {
        //a vista: saida do caixa
        QSqlQuery caixa;
        caixa.prepare("INSERT INTO tb_caixa (tipo, descricao, valor, data, usuario) "
                      "VALUES (?,?,?,?,?)");
        caixa.addBindValue("saida");
        caixa.addBindValue("Compra #" + QString::number(idCompra));
        caixa.addBindValue(total);
        caixa.addBindValue(data);
        caixa.addBindValue(sessao().nome.isEmpty() ? QString("-") : sessao().nome);
        caixa.exec();
    }

    registrarLog("Finalização", "Compras",
                 "compra #" + QString::number(idCompra) + " total: " +
                 QString::number(total, 'f', 2));
    limparTudo();
    QMessageBox::information(this, "Compras", "Compra finalizada com sucesso!");
}

//cancela a compra (limpa o carrinho)
void CompraWindow::on_pushButton_4_clicked()
{
    if (ui->tableWidget->rowCount() > 0 &&
        QMessageBox::question(this, "Compras", "Cancelar a compra e limpar os itens?") !=
        QMessageBox::Yes)
        return;
    limparTudo();
}
