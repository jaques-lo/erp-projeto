#include "relatoriowindow.h"
#include "ui_relatoriowindow.h"
#include <QtSql>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>

RelatorioWindow::RelatorioWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RelatorioWindow)
{
    ui->setupUi(this);
}

RelatorioWindow::~RelatorioWindow()
{
    delete ui;
}

//executa a consulta e mostra o resultado na tabela
void RelatorioWindow::montar(const QString &titulo, const QStringList &colunas,
                             const QString &sql)
{
    QSqlQuery q;
    if (!q.exec(sql)) {
        QMessageBox::warning(this, "Relatórios", "Erro ao gerar o relatório:\n" +
                             q.lastError().text());
        return;
    }

    ui->tableWidget->clear();
    ui->tableWidget->setColumnCount(colunas.size());
    ui->tableWidget->setHorizontalHeaderLabels(colunas);

    ui->tableWidget->setRowCount(0);
    while (q.next()) {
        int linha = ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(linha);
        for (int c = 0; c < colunas.size(); ++c) {
            QString texto = q.value(c).toString();
            //mostra valores monetarios com 2 casas
            if (colunas[c].contains("Valor") || colunas[c].contains("Total") ||
                colunas[c].contains("R$"))
                texto = QString::number(q.value(c).toDouble(), 'f', 2);
            QTableWidgetItem *item = new QTableWidgetItem(texto);
            item->setFlags(item->flags() & ~Qt::ItemIsEditable);
            ui->tableWidget->setItem(linha, c, item);
        }
    }
    ui->tableWidget->resizeColumnsToContents();

    //titulo com a quantidade de registros
    QString total;
    if (colunas.size() == 1)
        total = "";   //nao mexe
    ui->label_titulo->setText(titulo + "  —  " +
                              QString::number(ui->tableWidget->rowCount()) +
                              " registro(s)");
}

//gera o relatorio escolhido no combo
void RelatorioWindow::on_btn_gerar_clicked()
{
    int opcao = ui->comboBox->currentIndex();

    switch (opcao) {
    case 0:
        QMessageBox::information(this, "Relatórios",
                                 "Escolha um relatório no combo e clique em Gerar.");
        return;

    case 1:   //produtos com estoque baixo
        montar("Produtos com estoque baixo",
               {"Código", "Produto", "Categoria", "Qtd. atual", "Qtd. mínima"},
               "SELECT COALESCE(NULLIF(p.codigo_barras,''), p.id_produtos), p.descricao, "
               "COALESCE(c.nome,'-'), COALESCE(e.quantidade,0), p.estoque_minimo "
               "FROM tb_produtos p "
               "LEFT JOIN tb_categorias c ON c.id_categoria = p.id_categoria "
               "LEFT JOIN tb_estoque e ON e.id_produto = p.id_produtos "
               "WHERE COALESCE(e.quantidade,0) = 0 OR COALESCE(e.quantidade,0) <= p.estoque_minimo "
               "ORDER BY p.descricao");
        break;

    case 2: {  //estoque atual com valores
        double valorTotal = 0;
        QSqlQuery soma;
        if (soma.exec("SELECT COALESCE(SUM(COALESCE(e.quantidade,0) * p.preco),0) "
                      "FROM tb_produtos p LEFT JOIN tb_estoque e "
                      "ON e.id_produto = p.id_produtos") && soma.next())
            valorTotal = soma.value(0).toDouble();

        montar("Estoque atual (valor total: R$ " +
               QString::number(valorTotal, 'f', 2) + ")",
               {"Produto", "Categoria", "Qtd.", "Preço", "Valor (R$)"},
               "SELECT p.descricao, COALESCE(c.nome,'-'), COALESCE(e.quantidade,0), "
               "p.preco, COALESCE(e.quantidade,0) * p.preco "
               "FROM tb_produtos p "
               "LEFT JOIN tb_categorias c ON c.id_categoria = p.id_categoria "
               "LEFT JOIN tb_estoque e ON e.id_produto = p.id_produtos "
               "ORDER BY p.descricao");
        break;
    }

    case 3:   //produtos mais vendidos
        montar("Produtos mais vendidos",
               {"Produto", "Qtd. vendida", "Total vendido (R$)"},
               "SELECT p.descricao, SUM(iv.quantidade), SUM(iv.subtotal) "
               "FROM tb_itens_venda iv JOIN tb_produtos p "
               "ON p.id_produtos = iv.id_produto "
               "GROUP BY iv.id_produto ORDER BY SUM(iv.quantidade) DESC LIMIT 50");
        break;

    case 4:   //vendas por forma de pagamento
        montar("Vendas por forma de pagamento",
               {"Forma de pagamento", "Vendas", "Total (R$)"},
               "SELECT forma_pagamento, COUNT(*), SUM(total) FROM tb_vendas "
               "WHERE status='finalizada' GROUP BY forma_pagamento "
               "ORDER BY SUM(total) DESC");
        break;

    case 5:   //contas a pagar pendentes
        montar("Contas a pagar pendentes",
               {"Fornecedor", "Categoria", "Valor (R$)", "Vencimento", "Status"},
               "SELECT COALESCE(f.nome,'-'), p.categoria, p.valor, p.data_vencimento, "
               "p.status FROM tb_pagar p "
               "LEFT JOIN tb_fornecedores f ON f.id_fornecedor = p.id_fornecedor "
               "WHERE p.status != 'pago' ORDER BY p.data_vencimento");
        break;

    case 6:   //contas a receber pendentes
        montar("Contas a receber pendentes",
               {"Cliente", "Categoria", "Valor (R$)", "Vencimento", "Status"},
               "SELECT COALESCE(c.nome,'-'), r.categoria, r.valor, r.data_vencimento, "
               "r.status FROM tb_receber r "
               "LEFT JOIN tb_clientes c ON c.id_cliente = r.id_cliente "
               "WHERE r.status != 'pago' ORDER BY r.data_vencimento");
        break;

    case 7:   //clientes cadastrados
        montar("Clientes cadastrados",
               {"Nome", "CPF/CNPJ", "Telefone", "Cidade", "Cadastro"},
               "SELECT nome, cpf_cnpj, telefone, cidade, data_cadastro "
               "FROM tb_clientes WHERE ativo=1 ORDER BY nome");
        break;

    default:
        return;
    }
}

//exporta o relatorio mostrado na tabela para CSV
void RelatorioWindow::on_btn_exportar_clicked()
{
    if (ui->tableWidget->rowCount() == 0) {
        QMessageBox::information(this, "Relatórios",
                                 "Gere um relatório antes de exportar.");
        return;
    }

    QString caminho = QFileDialog::getSaveFileName(this, "Exportar Relatório",
                                                   "relatorio_erp.csv", "CSV (*.csv)");
    if (caminho.isEmpty())
        return;

    QFile arquivo(caminho);
    if (!arquivo.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Relatórios",
                             "Não foi possível criar o arquivo:\n" + caminho);
        return;
    }

    QTextStream out(&arquivo);
    out.setCodec("UTF-8");   //Qt 5: equivalente a setEncoding(QStringConverter::Utf8) do Qt 6

    QStringList cabecalho;
    for (int c = 0; c < ui->tableWidget->columnCount(); ++c)
        cabecalho << ui->tableWidget->horizontalHeaderItem(c)->text();
    out << cabecalho.join(";") << "\n";

    for (int l = 0; l < ui->tableWidget->rowCount(); ++l) {
        QStringList campos;
        for (int c = 0; c < ui->tableWidget->columnCount(); ++c) {
            QString texto = ui->tableWidget->item(l, c)->text();
            texto.replace("\"", "\"\"");
            campos << "\"" + texto + "\"";
        }
        out << campos.join(";") << "\n";
    }
    arquivo.close();

    QMessageBox::information(this, "Relatórios",
                             "Relatório exportado com sucesso!\n" + caminho);
}
