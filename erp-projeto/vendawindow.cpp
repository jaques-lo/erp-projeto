#include "vendawindow.h"
#include "ui_vendawindow.h"
#include "loghelper.h"
#include "telashelper.h"
#include <QtSql>
#include <QDate>
#include <QInputDialog>
#include <QMessageBox>

VendaWindow::VendaWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::VendaWindow)
{
    ui->setupUi(this);
    ui->doubleSpinBox->setMaximum(99999999.99);  //total (o padrao do Qt para em 99.99)
    ui->doubleSpinBox->setReadOnly(true);
    ui->doubleSpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);

    //digitou o codigo do produto e apertou Enter = adicionar item
    connect(ui->lineEdit, &QLineEdit::returnPressed, this, &VendaWindow::on_pushButton_clicked);
}

VendaWindow::~VendaWindow()
{
    delete ui;
}

//soma os subtotais dos itens do carrinho
double VendaWindow::totalCarrinho() const
{
    double total = 0;
    for (int l = 0; l < ui->tableWidget->rowCount(); ++l)
        total += ui->tableWidget->item(l, 4)->text().toDouble();
    return total;
}

//limpa a tela para uma nova venda
void VendaWindow::limparTudo()
{
    ui->tableWidget->setRowCount(0);
    ui->lineEdit->clear();
    ui->comboBox->setCurrentIndex(0);
    ui->doubleSpinBox->setValue(0);
}

//adiciona o produto do campo "codigo do produto" no carrinho
void VendaWindow::on_pushButton_clicked()
{
    QString texto = ui->lineEdit->text().trimmed();
    if (texto.isEmpty()) {
        QMessageBox::warning(this, "Vendas", "Digite o código do produto "
                             "(ou parte da descrição).");
        return;
    }

    QSqlQuery q;
    q.prepare("SELECT id_produtos, COALESCE(NULLIF(codigo_barras,''), codigo_barras), "
              "descricao, preco, ativo FROM tb_produtos "
              "WHERE codigo_barras = ? OR descricao LIKE ? "
              "ORDER BY descricao LIMIT 1");
    q.addBindValue(texto);
    q.addBindValue("%" + texto + "%");
    if (!q.exec() || !q.next()) {
        QMessageBox::information(this, "Vendas", "Produto não encontrado:\n" + texto);
        return;
    }

    int idProduto = q.value(0).toInt();
    QString codigo = q.value(1).toString().isEmpty()
                     ? QString::number(idProduto) : q.value(1).toString();
    QString descricao = q.value(2).toString();
    double preco = q.value(3).toDouble();
    if (q.value(4).toInt() == 0) {
        QMessageBox::warning(this, "Vendas", "Este produto está inativo.");
        return;
    }

    //estoque disponivel
    int estoque = 0;
    QSqlQuery est;
    est.prepare("SELECT quantidade FROM tb_estoque WHERE id_produto=?");
    est.addBindValue(idProduto);
    if (est.exec() && est.next())
        estoque = est.value(0).toInt();
    if (estoque <= 0) {
        QMessageBox::warning(this, "Vendas", "Produto sem estoque:\n" + descricao);
        return;
    }

    bool ok = false;
    int quantidade = QInputDialog::getInt(this, "Adicionar Item",
                                          "Quantidade (estoque: " +
                                          QString::number(estoque) + "):",
                                          1, 1, estoque, 1, &ok);
    if (!ok || quantidade <= 0)
        return;

    //se o produto ja esta no carrinho, soma a quantidade
    for (int l = 0; l < ui->tableWidget->rowCount(); ++l) {
        if (ui->tableWidget->item(l, 0)->data(Qt::UserRole).toInt() == idProduto) {
            int atual = ui->tableWidget->item(l, 2)->text().toInt();
            int nova = atual + quantidade;
            if (nova > estoque) {
                QMessageBox::warning(this, "Vendas",
                                     "Estoque insuficiente.\nDisponível: " +
                                     QString::number(estoque));
                return;
            }
            ui->tableWidget->item(l, 2)->setText(QString::number(nova));
            ui->tableWidget->item(l, 4)->setText(
                        QString::number(preco * nova, 'f', 2));
            ui->doubleSpinBox->setValue(totalCarrinho());
            ui->lineEdit->clear();
            return;
        }
    }

    double subtotal = preco * quantidade;
    int linha = ui->tableWidget->rowCount();
    ui->tableWidget->insertRow(linha);
    ui->tableWidget->setItem(linha, 0, new QTableWidgetItem(codigo));               //Codigo
    ui->tableWidget->setItem(linha, 1, new QTableWidgetItem(QString::number(preco, 'f', 2))); //Preco
    ui->tableWidget->setItem(linha, 2, new QTableWidgetItem(QString::number(quantidade)));    //Qtd
    ui->tableWidget->setItem(linha, 3, new QTableWidgetItem(descricao));            //Descricao
    ui->tableWidget->setItem(linha, 4, new QTableWidgetItem(QString::number(subtotal, 'f', 2))); //Subtotal
    ui->tableWidget->item(linha, 0)->setData(Qt::UserRole, idProduto);
    for (int c = 0; c < ui->tableWidget->columnCount(); ++c)
        ui->tableWidget->item(linha, c)->setFlags(ui->tableWidget->item(linha, c)->flags()
                                                  & ~Qt::ItemIsEditable);

    ui->doubleSpinBox->setValue(totalCarrinho());
    ui->lineEdit->clear();
}

//remove o item selecionado do carrinho
void VendaWindow::on_pushButton_2_clicked()
{
    int linha = ui->tableWidget->currentRow();
    if (linha < 0) {
        QMessageBox::warning(this, "Vendas", "Selecione um item para remover.");
        return;
    }
    ui->tableWidget->removeRow(linha);
    ui->doubleSpinBox->setValue(totalCarrinho());
}

//finaliza a venda: grava venda + itens, baixa no estoque e lanca no caixa
void VendaWindow::on_pushButton_3_clicked()
{
    if (ui->tableWidget->rowCount() == 0) {
        QMessageBox::warning(this, "Vendas", "Adicione pelo menos um item.");
        return;
    }
    if (QMessageBox::question(this, "Vendas", "Finalizar esta venda?") !=
        QMessageBox::Yes)
        return;

    //confere o estoque de todos os itens antes de gravar
    for (int l = 0; l < ui->tableWidget->rowCount(); ++l) {
        int idProduto = ui->tableWidget->item(l, 0)->data(Qt::UserRole).toInt();
        int pedido = ui->tableWidget->item(l, 2)->text().toInt();
        int estoque = 0;
        QSqlQuery est;
        est.prepare("SELECT quantidade FROM tb_estoque WHERE id_produto=?");
        est.addBindValue(idProduto);
        if (est.exec() && est.next())
            estoque = est.value(0).toInt();
        if (estoque < pedido) {
            QMessageBox::warning(this, "Vendas",
                                 "Estoque insuficiente para:\n" +
                                 ui->tableWidget->item(l, 3)->text() +
                                 "\nDisponível: " + QString::number(estoque));
            return;
        }
    }

    double total = totalCarrinho();
    QString data = QDate::currentDate().toString("dd/MM/yyyy");
    QString forma = ui->comboBox->currentText();   //Dinheiro / Credito / Debito / Pix

    //1) cabecalho da venda (sem tela de cliente: consumidor final = 0)
    QSqlQuery venda;
    venda.prepare("INSERT INTO tb_vendas (id_cliente, id_usuario, data, subtotal, desconto, "
                  "total, forma_pagamento, status) VALUES (?,?,?,?,?,?,?,?)");
    venda.addBindValue(0);
    venda.addBindValue(sessao().id);
    venda.addBindValue(data);
    venda.addBindValue(total);
    venda.addBindValue(0);
    venda.addBindValue(total);
    venda.addBindValue(forma);
    venda.addBindValue("finalizada");
    if (!venda.exec()) {
        QMessageBox::warning(this, "Vendas", "Erro ao gravar a venda:\n" +
                             venda.lastError().text());
        return;
    }
    int idVenda = venda.lastInsertId().toInt();

    //2) itens + baixa no estoque + movimentacao
    for (int l = 0; l < ui->tableWidget->rowCount(); ++l) {
        int idProduto = ui->tableWidget->item(l, 0)->data(Qt::UserRole).toInt();
        double preco = ui->tableWidget->item(l, 1)->text().toDouble();
        int quantidade = ui->tableWidget->item(l, 2)->text().toInt();
        double subtotal = ui->tableWidget->item(l, 4)->text().toDouble();

        QSqlQuery item;
        item.prepare("INSERT INTO tb_itens_venda (id_venda, id_produto, quantidade, "
                     "valor_unitario, subtotal) VALUES (?,?,?,?,?)");
        item.addBindValue(idVenda);
        item.addBindValue(idProduto);
        item.addBindValue(quantidade);
        item.addBindValue(preco);
        item.addBindValue(subtotal);
        item.exec();

        //baixa no estoque
        QSqlQuery baixa;
        baixa.prepare("UPDATE tb_estoque SET quantidade = quantidade - ? WHERE id_produto=?");
        baixa.addBindValue(quantidade);
        baixa.addBindValue(idProduto);
        baixa.exec();

        //historico de movimentacao
        QSqlQuery mov;
        mov.prepare("INSERT INTO tb_movimentacao_estoque (id_produto, tipo, quantidade, "
                    "data, usuario, observacao) VALUES (?,?,?,?,?,?)");
        mov.addBindValue(idProduto);
        mov.addBindValue("saída");
        mov.addBindValue(quantidade);
        mov.addBindValue(data);
        mov.addBindValue(sessao().nome.isEmpty() ? QString("-") : sessao().nome);
        mov.addBindValue("venda #" + QString::number(idVenda));
        mov.exec();
    }

    //3) entrada no caixa (venda a vista)
    QSqlQuery caixa;
    caixa.prepare("INSERT INTO tb_caixa (tipo, descricao, valor, data, usuario) "
                  "VALUES (?,?,?,?,?)");
    caixa.addBindValue("entrada");
    caixa.addBindValue("Venda #" + QString::number(idVenda) + " (" + forma + ")");
    caixa.addBindValue(total);
    caixa.addBindValue(data);
    caixa.addBindValue(sessao().nome.isEmpty() ? QString("-") : sessao().nome);
    caixa.exec();

    registrarLog("Finalização", "Vendas",
                 "venda #" + QString::number(idVenda) + " total: " +
                 QString::number(total, 'f', 2) + " (" + forma + ")");
    limparTudo();
    QMessageBox::information(this, "Vendas", "Venda finalizada com sucesso!");
}

//cancela a venda (limpa o carrinho)
void VendaWindow::on_pushButton_4_clicked()
{
    if (ui->tableWidget->rowCount() > 0 &&
        QMessageBox::question(this, "Vendas", "Cancelar a venda e limpar os itens?") !=
        QMessageBox::Yes)
        return;
    limparTudo();
}
