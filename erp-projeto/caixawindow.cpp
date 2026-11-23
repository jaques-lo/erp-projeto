#include "caixawindow.h"
#include "ui_caixawindow.h"
#include "loghelper.h"
#include "telashelper.h"
#include <QtSql>
#include <QDateTime>
#include <QInputDialog>
#include <QMessageBox>

//le uma configuracao salva no banco (tb_config)
static QString lerConfig(const QString &chave)
{
    QSqlQuery q;
    q.prepare("SELECT valor FROM tb_config WHERE chave=?");
    q.addBindValue(chave);
    if (q.exec() && q.next())
        return q.value(0).toString();
    return QString();
}

//grava uma configuracao (tb_config)
static void gravarConfig(const QString &chave, const QString &valor)
{
    QSqlQuery q;
    q.prepare("INSERT INTO tb_config (chave, valor) VALUES (?,?) "
              "ON CONFLICT(chave) DO UPDATE SET valor=excluded.valor");
    q.addBindValue(chave);
    q.addBindValue(valor);
    q.exec();
}

CaixaWindow::CaixaWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CaixaWindow)
{
    ui->setupUi(this);

    //os campos de data e diferenca so exibem
    ui->dateTimeEdit->setReadOnly(true);
    ui->doubleSpinBox_3->setMaximum(99999999.99);
    ui->doubleSpinBox_3->setReadOnly(true);
    ui->doubleSpinBox_3->setButtonSymbols(QAbstractSpinBox::NoButtons);
    ui->doubleSpinBox->setMaximum(99999999.99);    //valor de abertura
    ui->doubleSpinBox_2->setMaximum(99999999.99);  //valor de fechamento
    ui->dateTimeEdit->setDateTime(QDateTime::currentDateTime());

    carregarEstado();
    carregarResumo();
}

CaixaWindow::~CaixaWindow()
{
    delete ui;
}

//le o status atual do caixa salvo no banco
void CaixaWindow::carregarEstado()
{
    QString status = lerConfig("caixa_status");
    if (status.isEmpty())
        status = "fechado";
    int i = ui->comboBox->findText(status);
    ui->comboBox->setCurrentIndex(i >= 0 ? i : 1);

    QString abertura = lerConfig("caixa_abertura");
    if (!abertura.isEmpty())
        ui->dateTimeEdit->setDateTime(QDateTime::fromString(abertura, "dd/MM/yyyy HH:mm:ss"));

    ui->doubleSpinBox->setValue(lerConfig("caixa_valor_abertura").toDouble());

    QString fechamento = lerConfig("caixa_fechamento");
    if (!fechamento.isEmpty() && status == "fechado")
        ui->doubleSpinBox_2->setValue(lerConfig("caixa_valor_fechamento").toDouble());
    else
        ui->doubleSpinBox_2->setValue(0);

    ui->doubleSpinBox_3->setValue(lerConfig("caixa_diferenca").toDouble());
}

void CaixaWindow::salvarEstado() const
{
    gravarConfig("caixa_status", ui->comboBox->currentText());
    gravarConfig("caixa_abertura", ui->dateTimeEdit->dateTime().toString("dd/MM/yyyy HH:mm:ss"));
    gravarConfig("caixa_valor_abertura", QString::number(ui->doubleSpinBox->value(), 'f', 2));
    gravarConfig("caixa_valor_fechamento", QString::number(ui->doubleSpinBox_2->value(), 'f', 2));
    gravarConfig("caixa_diferenca", QString::number(ui->doubleSpinBox_3->value(), 'f', 2));
}

//quanto deveria ter no caixa: abertura + entradas - saidas
double CaixaWindow::saldoEsperado() const
{
    double saldo = ui->doubleSpinBox->value();   //abertura

    QSqlQuery q;
    if (q.exec("SELECT COALESCE(SUM(CASE WHEN tipo IN ('entrada','suprimento') THEN valor "
               "WHEN tipo IN ('saida','sangria') THEN -valor ELSE 0 END),0) FROM tb_caixa")) {
        if (q.next())
            saldo += q.value(0).toDouble();
    }
    return saldo;
}

//vendas agrupadas por forma de pagamento (Dinheiro / Cartao / Pix / Total)
void CaixaWindow::carregarResumo()
{
    double dinheiro = 0, credito = 0, debito = 0, pix = 0;
    QSqlQuery q;
    if (q.exec("SELECT forma_pagamento, COALESCE(SUM(total),0) FROM tb_vendas "
               "WHERE status='finalizada' GROUP BY forma_pagamento")) {
        while (q.next()) {
            QString forma = q.value(0).toString();
            double valor = q.value(1).toDouble();
            if (forma == "Dinheiro") dinheiro += valor;
            else if (forma == "Crédito" || forma == "Débito") credito += valor;
            else if (forma == "Pix") pix += valor;
        }
    }

    ui->tableWidget->setRowCount(1);
    ui->tableWidget->setItem(0, 0, new QTableWidgetItem(QString::number(dinheiro, 'f', 2)));
    ui->tableWidget->setItem(0, 1, new QTableWidgetItem(QString::number(credito, 'f', 2)));
    ui->tableWidget->setItem(0, 2, new QTableWidgetItem(QString::number(pix, 'f', 2)));
    ui->tableWidget->setItem(0, 3, new QTableWidgetItem(
                                 QString::number(dinheiro + credito + debito + pix, 'f', 2)));
    for (int c = 0; c < ui->tableWidget->columnCount(); ++c) {
        ui->tableWidget->item(0, c)->setFlags(ui->tableWidget->item(0, c)->flags()
                                              & ~Qt::ItemIsEditable);
        ui->tableWidget->item(0, c)->setTextAlignment(Qt::AlignCenter);
    }
}

//abre o caixa com um valor de fundo
void CaixaWindow::on_pushButton_clicked()
{
    if (ui->comboBox->currentText() == "aberto") {
        QMessageBox::information(this, "Caixa", "O caixa já está aberto.");
        return;
    }

    bool ok = false;
    double valor = QInputDialog::getDouble(this, "Abrir Caixa",
                                           "Valor de abertura (troco):",
                                           ui->doubleSpinBox->value(), 0, 99999999, 2, &ok);
    if (!ok)
        return;

    ui->doubleSpinBox->setValue(valor);
    ui->dateTimeEdit->setDateTime(QDateTime::currentDateTime());
    ui->comboBox->setCurrentText("aberto");
    ui->doubleSpinBox_2->setValue(0);
    ui->doubleSpinBox_3->setValue(0);
    salvarEstado();

    registrarLog("Abertura", "Caixa", "valor: " + QString::number(valor, 'f', 2));
    QMessageBox::information(this, "Caixa", "Caixa aberto com sucesso.");
}

//retira dinheiro do caixa
void CaixaWindow::on_pushButton_2_clicked()
{
    if (ui->comboBox->currentText() != "aberto") {
        QMessageBox::warning(this, "Caixa", "Abra o caixa antes de fazer sangria.");
        return;
    }

    bool ok = false;
    double valor = QInputDialog::getDouble(this, "Sangria", "Valor da sangria:",
                                           0, 0.01, 99999999, 2, &ok);
    if (!ok || valor <= 0)
        return;

    QSqlQuery q;
    q.prepare("INSERT INTO tb_caixa (tipo, descricao, valor, data, usuario) VALUES (?,?,?,?,?)");
    q.addBindValue("sangria");
    q.addBindValue("Sangria do caixa");
    q.addBindValue(valor);
    q.addBindValue(QDateTime::currentDateTime().toString("dd/MM/yyyy HH:mm:ss"));
    q.addBindValue(sessao().nome.isEmpty() ? QString("-") : sessao().nome);
    if (!q.exec()) {
        QMessageBox::warning(this, "Caixa", "Erro ao registrar:\n" + q.lastError().text());
        return;
    }

    registrarLog("Sangria", "Caixa", "valor: " + QString::number(valor, 'f', 2));
    carregarResumo();
    QMessageBox::information(this, "Caixa", "Sangria registrada.\nSaldo esperado: " +
                             QString::number(saldoEsperado(), 'f', 2));
}

//coloca dinheiro no caixa (troco)
void CaixaWindow::on_pushButton_4_clicked()
{
    if (ui->comboBox->currentText() != "aberto") {
        QMessageBox::warning(this, "Caixa", "Abra o caixa antes de fazer suprimento.");
        return;
    }

    bool ok = false;
    double valor = QInputDialog::getDouble(this, "Suprimento", "Valor do suprimento:",
                                           0, 0.01, 99999999, 2, &ok);
    if (!ok || valor <= 0)
        return;

    QSqlQuery q;
    q.prepare("INSERT INTO tb_caixa (tipo, descricao, valor, data, usuario) VALUES (?,?,?,?,?)");
    q.addBindValue("suprimento");
    q.addBindValue("Suprimento de caixa");
    q.addBindValue(valor);
    q.addBindValue(QDateTime::currentDateTime().toString("dd/MM/yyyy HH:mm:ss"));
    q.addBindValue(sessao().nome.isEmpty() ? QString("-") : sessao().nome);
    if (!q.exec()) {
        QMessageBox::warning(this, "Caixa", "Erro ao registrar:\n" + q.lastError().text());
        return;
    }

    registrarLog("Suprimento", "Caixa", "valor: " + QString::number(valor, 'f', 2));
    carregarResumo();
    QMessageBox::information(this, "Caixa", "Suprimento registrado.\nSaldo esperado: " +
                             QString::number(saldoEsperado(), 'f', 2));
}

//fecha o caixa conferindo o dinheiro contado
void CaixaWindow::on_pushButton_3_clicked()
{
    if (ui->comboBox->currentText() != "aberto") {
        QMessageBox::warning(this, "Caixa", "O caixa não está aberto.");
        return;
    }
    if (QMessageBox::question(this, "Caixa", "Fechar o caixa?") != QMessageBox::Yes)
        return;

    double esperado = saldoEsperado();
    bool ok = false;
    double contado = QInputDialog::getDouble(this, "Fechar Caixa",
                                             "Dinheiro contado no caixa\n"
                                             "(esperado: " +
                                             QString::number(esperado, 'f', 2) + "):",
                                             esperado, 0, 99999999, 2, &ok);
    if (!ok)
        return;

    double diferenca = contado - esperado;
    ui->doubleSpinBox_2->setValue(contado);
    ui->doubleSpinBox_3->setValue(diferenca);
    ui->comboBox->setCurrentText("fechado");
    ui->dateTimeEdit->setDateTime(QDateTime::currentDateTime());
    salvarEstado();

    registrarLog("Fechamento", "Caixa",
                 "esperado: " + QString::number(esperado, 'f', 2) +
                 " contado: " + QString::number(contado, 'f', 2) +
                 " diferença: " + QString::number(diferenca, 'f', 2));
    carregarResumo();
    QMessageBox::information(this, "Caixa",
                             "Caixa fechado.\nDiferença: " +
                             QString::number(diferenca, 'f', 2));
}
