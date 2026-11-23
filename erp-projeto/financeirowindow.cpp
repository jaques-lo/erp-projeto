#include "financeirowindow.h"
#include "ui_financeirowindow.h"
#include "contaspagarwindow.h"
#include "contasreceberwindow.h"
#include "relatoriowindow.h"
#include "telashelper.h"
#include <QtSql>
#include <QShowEvent>

FinanceiroWindow::FinanceiroWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FinanceiroWindow)
{
    ui->setupUi(this);

    //os tres valores do painel so exibem
    for (QDoubleSpinBox *s : {ui->doubleSpinBox, ui->doubleSpinBox_2, ui->doubleSpinBox_3}) {
        s->setMaximum(99999999.99);
        s->setMinimum(-99999999.99);
        s->setReadOnly(true);
        s->setButtonSymbols(QAbstractSpinBox::NoButtons);
    }

    atualizarPainel();
}

FinanceiroWindow::~FinanceiroWindow()
{
    delete ui;
}

//sempre que a tela for mostrada, recalcula os totais
void FinanceiroWindow::showEvent(QShowEvent *evento)
{
    QWidget::showEvent(evento);
    atualizarPainel();
}

//total a receber, total a pagar, saldo e contas proximas do vencimento
void FinanceiroWindow::atualizarPainel()
{
    double receber = 0, pagar = 0;

    QSqlQuery q;
    if (q.exec("SELECT COALESCE(SUM(valor),0) FROM tb_receber WHERE status != 'pago'")) {
        if (q.next())
            receber = q.value(0).toDouble();
    }
    if (q.exec("SELECT COALESCE(SUM(valor),0) FROM tb_pagar WHERE status != 'pago'")) {
        if (q.next())
            pagar = q.value(0).toDouble();
    }

    ui->doubleSpinBox->setValue(receber);              //total a receber
    ui->doubleSpinBox_2->setValue(pagar);              //total a pagar
    ui->doubleSpinBox_3->setValue(receber - pagar);    //saldo

    //contas pendentes mais proximas do vencimento (pagar e receber juntas)
    QSqlQuery lista;
    if (!lista.exec(
            "SELECT 'A Pagar' AS origem, COALESCE(f.nome,'-') AS pessoa, p.categoria, "
            "p.valor, p.data_vencimento, p.status "
            "FROM tb_pagar p LEFT JOIN tb_fornecedores f ON f.id_fornecedor = p.id_fornecedor "
            "WHERE p.status != 'pago' "
            "UNION ALL "
            "SELECT 'A Receber', COALESCE(c.nome,'-'), r.categoria, r.valor, "
            "r.data_vencimento, r.status "
            "FROM tb_receber r LEFT JOIN tb_clientes c ON c.id_cliente = r.id_cliente "
            "WHERE r.status != 'pago' "
            "ORDER BY data_vencimento LIMIT 30")) {
        return;
    }

    const QStringList colunas = {"Tipo", "Pessoa", "Categoria", "Valor",
                                 "Vencimento", "Status"};
    ui->tableWidget->clear();
    ui->tableWidget->setColumnCount(colunas.size());
    ui->tableWidget->setHorizontalHeaderLabels(colunas);

    ui->tableWidget->setRowCount(0);
    while (lista.next()) {
        int linha = ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(linha);
        for (int c = 0; c < colunas.size(); ++c) {
            QString texto = lista.value(c).toString();
            if (c == 3)
                texto = QString::number(lista.value(c).toDouble(), 'f', 2);
            QTableWidgetItem *item = new QTableWidgetItem(texto);
            item->setFlags(item->flags() & ~Qt::ItemIsEditable);
            ui->tableWidget->setItem(linha, c, item);
        }
    }
    ui->tableWidget->resizeColumnsToContents();
}

//botoes de navegacao do modulo financeiro
void FinanceiroWindow::on_pushButton_clicked()
{
    abrirTela<ContasPagarWindow>();
}

void FinanceiroWindow::on_pushButton_2_clicked()
{
    abrirTela<ContasReceberWindow>();
}

void FinanceiroWindow::on_pushButton_3_clicked()
{
    abrirTela<RelatorioWindow>();
}
