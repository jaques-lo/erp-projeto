#include "estoquewindow.h"
#include "ui_estoquewindow.h"
#include "movimentacaoestoquewindow.h"
#include "telashelper.h"
#include "loghelper.h"
#include <QtSql>
#include <QMessageBox>
#include <QInputDialog>

EstoqueWindow::EstoqueWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EstoqueWindow)
{
    ui->setupUi(this);
    //mostra a situacao atual do estoque ao abrir
    carregarTabela();
}

EstoqueWindow::~EstoqueWindow()
{
    delete ui;
}

//preenche a tabela com todos os produtos e a quantidade em estoque
void EstoqueWindow::carregarTabela(const QString &filtroProduto,
                                   const QString &filtroCategoria)
{
    QSqlQuery q;
    QString sql =
        "SELECT p.id_produtos, COALESCE(p.codigo_barras,''), p.descricao, "
        "COALESCE(c.nome,''), COALESCE(e.quantidade,0), COALESCE(p.estoque_minimo,0) "
        "FROM tb_produtos p "
        "LEFT JOIN tb_categorias c ON c.id_categoria = p.id_categoria "
        "LEFT JOIN tb_estoque e ON e.id_produto = p.id_produtos WHERE 1=1";
    if (!filtroProduto.isEmpty())
        sql += " AND p.descricao LIKE '%" + filtroProduto + "%'";
    if (!filtroCategoria.isEmpty())
        sql += " AND c.nome LIKE '%" + filtroCategoria + "%'";
    sql += " ORDER BY p.descricao";

    if (!q.exec(sql)) {
        QMessageBox::warning(this, "Estoque", "Erro ao consultar:\n" + q.lastError().text());
        return;
    }

    ui->tableWidget->setRowCount(0);
    while (q.next()) {
        int linha = ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(linha);

        int qtdAtual = q.value(4).toInt();
        int qtdMin   = q.value(5).toInt();
        //status: zerado / abaixo do minimo / OK
        QString status = qtdAtual <= 0 ? "SEM ESTOQUE"
                       : (qtdMin > 0 && qtdAtual < qtdMin ? "BAIXO" : "OK");

        ui->tableWidget->setItem(linha, 0, new QTableWidgetItem(q.value(1).toString())); //Codigo
        ui->tableWidget->setItem(linha, 1, new QTableWidgetItem(q.value(2).toString())); //Produto
        ui->tableWidget->setItem(linha, 2, new QTableWidgetItem(q.value(3).toString())); //Categoria
        ui->tableWidget->setItem(linha, 3, new QTableWidgetItem(QString::number(qtdAtual))); //Qtd atual
        ui->tableWidget->setItem(linha, 4, new QTableWidgetItem(QString::number(qtdMin)));   //Qtd min
        ui->tableWidget->setItem(linha, 5, new QTableWidgetItem(status));                    //Status

        //guarda o id do produto na primeira coluna (para o ajuste)
        ui->tableWidget->item(linha, 0)->setData(Qt::UserRole, q.value(0).toInt());
        //nao deixa o usuario editar direto na tabela
        for (int c = 0; c < ui->tableWidget->columnCount(); ++c)
            ui->tableWidget->item(linha, c)->setFlags(ui->tableWidget->item(linha, c)->flags()
                                                      & ~Qt::ItemIsEditable);
    }
}

//pesquisa por produto e/ou categoria
void EstoqueWindow::on_pushButton_clicked()
{
    carregarTabela(ui->lineEdit->text().trimmed(), ui->lineEdit_2->text().trimmed());
}

//ajusta a quantidade de estoque do produto selecionado na tabela
void EstoqueWindow::on_pushButton_2_clicked()
{
    int linha = ui->tableWidget->currentRow();
    if (linha < 0) {
        QMessageBox::warning(this, "Estoque", "Selecione um produto na tabela para ajustar.");
        return;
    }

    QTableWidgetItem *item = ui->tableWidget->item(linha, 0);
    int idProduto = item->data(Qt::UserRole).toInt();
    QString produto = ui->tableWidget->item(linha, 1)->text();
    int atual = ui->tableWidget->item(linha, 3)->text().toInt();

    bool ok = false;
    int novo = QInputDialog::getInt(this, "Ajustar Estoque",
                                    "Nova quantidade de \"" + produto + "\"\n"
                                    "(atual: " + QString::number(atual) + "):",
                                    atual, 0, 999999, 1, &ok);
    if (!ok || novo == atual)
        return;

    QSqlQuery q;
    q.prepare("UPDATE tb_estoque SET quantidade=? WHERE id_produto=?");
    q.addBindValue(novo);
    q.addBindValue(idProduto);
    if (!q.exec()) {
        QMessageBox::warning(this, "Estoque", "Erro ao ajustar:\n" + q.lastError().text());
        return;
    }
    //se o produto ainda nao tinha linha de estoque, cria
    if (q.numRowsAffected() == 0) {
        QSqlQuery i;
        i.prepare("INSERT INTO tb_estoque (id_produto, quantidade, localizacao) VALUES (?,?,'')");
        i.addBindValue(idProduto);
        i.addBindValue(novo);
        i.exec();
    }

    //registra a movimentacao (ajuste) no historico
    QSqlQuery m;
    m.prepare("INSERT INTO tb_movimentacao_estoque (id_produto, tipo, quantidade, data, "
              "usuario, observacao) VALUES (?,?,?,?,?,?)");
    m.addBindValue(idProduto);
    m.addBindValue("ajuste");
    m.addBindValue(qAbs(novo - atual));
    m.addBindValue(QDate::currentDate().toString("dd/MM/yyyy"));
    m.addBindValue(sessao().nome.isEmpty() ? QString("-") : sessao().nome);
    m.addBindValue("ajuste manual de " + QString::number(atual) + " para " +
                   QString::number(novo));
    m.exec();

    registrarLog("Ajuste de estoque", "Estoque",
                 produto + ": " + QString::number(atual) + " -> " + QString::number(novo));
    carregarTabela(ui->lineEdit->text().trimmed(), ui->lineEdit_2->text().trimmed());
    QMessageBox::information(this, "Estoque", "Estoque ajustado com sucesso.");
}

//botao "Ver movimentacoes": abre o historico de movimentacao do estoque
void EstoqueWindow::on_pushButton_3_clicked()
{
    abrirTela<MovimentacaoEstoqueWindow>();
}
