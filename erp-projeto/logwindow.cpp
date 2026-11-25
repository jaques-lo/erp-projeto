#include "logwindow.h"
#include "ui_logwindow.h"
#include <QtSql>
#include <QDate>
#include <QFileDialog>
#include <QTextStream>
#include <QStringConverter>
#include <QMessageBox>

LogWindow::LogWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LogWindow)
{
    ui->setupUi(this);

    //filtro de data: deixa o usuario escolher "todas" (data minima com texto proprio)
    ui->dateEdit->setMinimumDate(QDate(2000, 1, 1));
    ui->dateEdit->setSpecialValueText("(todas as datas)");
    ui->dateEdit->setDate(ui->dateEdit->minimumDate());

    //opcao "Todas" no filtro de acao
    ui->comboBox->insertItem(0, "Todas");

    //carrega tudo ao abrir
    carregar(QString(), QString(), QDate());
}

LogWindow::~LogWindow()
{
    delete ui;
}

//preenche a tabela aplicando os filtros informados
void LogWindow::carregar(const QString &usuario, const QString &acao, const QDate &data)
{
    QString sql = "SELECT data, acao, usuario, modulo, detalhes FROM tb_logs WHERE 1=1";
    if (!usuario.isEmpty())
        sql += " AND usuario LIKE '%" + usuario + "%'";
    if (!acao.isEmpty())
        sql += " AND acao = '" + acao + "'";
    if (data.isValid())
        sql += " AND data LIKE '" + data.toString("dd/MM/yyyy") + "%'";
    sql += " ORDER BY id_log DESC LIMIT 500";

    QSqlQuery q;
    if (!q.exec(sql)) {
        QMessageBox::warning(this, "Logs", "Erro ao consultar:\n" + q.lastError().text());
        return;
    }

    ui->tableWidget->setRowCount(0);
    while (q.next()) {
        int linha = ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(linha);
        for (int c = 0; c < 5; ++c) {
            QTableWidgetItem *item = new QTableWidgetItem(q.value(c).toString());
            item->setFlags(item->flags() & ~Qt::ItemIsEditable);
            ui->tableWidget->setItem(linha, c, item);
        }
    }
}

//aplica os filtros da barra superior
void LogWindow::on_pushButton_clicked()
{
    QString acao = ui->comboBox->currentText();
    if (acao == "Todas")
        acao.clear();

    QDate data;
    if (ui->dateEdit->date() != ui->dateEdit->minimumDate())
        data = ui->dateEdit->date();

    carregar(ui->lineEdit->text().trimmed(), acao, data);
}

//exporta o que esta na tabela para um arquivo CSV (abre no Excel)
void LogWindow::on_pushButton_2_clicked()
{
    if (ui->tableWidget->rowCount() == 0) {
        QMessageBox::information(this, "Logs", "Não há registros para exportar.");
        return;
    }

    QString caminho = QFileDialog::getSaveFileName(this, "Exportar Logs",
                                                   "logs_erp.csv", "CSV (*.csv)");
    if (caminho.isEmpty())
        return;

    QFile arquivo(caminho);
    if (!arquivo.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Logs", "Não foi possível criar o arquivo:\n" + caminho);
        return;
    }

    QTextStream out(&arquivo);
    out.setEncoding(QStringConverter::Utf8);
    //cabecalho
    QStringList cabecalho;
    for (int c = 0; c < ui->tableWidget->columnCount(); ++c)
        cabecalho << ui->tableWidget->horizontalHeaderItem(c)->text();
    out << cabecalho.join(";") << "\n";

    //linhas
    for (int l = 0; l < ui->tableWidget->rowCount(); ++l) {
        QStringList campos;
        for (int c = 0; c < ui->tableWidget->columnCount(); ++c) {
            QString texto = ui->tableWidget->item(l, c)->text();
            texto.replace("\"", "\"\"");   //escapa aspas do CSV
            campos << "\"" + texto + "\"";
        }
        out << campos.join(";") << "\n";
    }
    arquivo.close();

    QMessageBox::information(this, "Logs",
                             "Registros exportados com sucesso!\n" + caminho);
}
