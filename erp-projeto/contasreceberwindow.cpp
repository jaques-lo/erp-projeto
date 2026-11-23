#include "contasreceberwindow.h"
#include "ui_contasreceberwindow.h"
#include "loghelper.h"
#include <QtSql>
#include <QMessageBox>

ContasReceberWindow::ContasReceberWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ContasReceberWindow)
{
    ui->setupUi(this);

    ui->doubleSpinBox->setMaximum(99999999.99);   //valor (o padrao do Qt para em 99.99)
    ui->lineEdit->setReadOnly(true);              //ID so exibe
    ui->dateEdit->setDate(QDate::currentDate());          //emissao
    ui->dateEdit_2->setDate(QDate::currentDate().addDays(30)); //vencimento
    //data de pagamento vazia = usa a data minima com um texto diferente
    ui->dateEdit_3->setMinimumDate(QDate(2000, 1, 1));
    ui->dateEdit_3->setSpecialValueText("(sem pagamento)");

    carregarLista();

    //ao clicar numa linha da lista, carrega a conta nos campos
    connect(ui->tableWidget, &QTableWidget::itemSelectionChanged, this, [this]() {
        int linha = ui->tableWidget->currentRow();
        if (linha < 0)
            return;
        idAtual = ui->tableWidget->item(linha, 0)->data(Qt::UserRole).toInt();
        ui->lineEdit->setText(ui->tableWidget->item(linha, 0)->text());   //ID
        ui->lineEdit_2->setText(ui->tableWidget->item(linha, 1)->text()); //Cliente
        int i = ui->comboBox->findText(ui->tableWidget->item(linha, 2)->text());
        if (i >= 0) ui->comboBox->setCurrentIndex(i);                    //Categoria
        ui->doubleSpinBox->setValue(ui->tableWidget->item(linha, 3)->text().toDouble()); //Valor
        ui->dateEdit->setDate(QDate::fromString(ui->tableWidget->item(linha, 4)->text(),
                                                "dd/MM/yyyy"));          //Emissao
        ui->dateEdit_2->setDate(QDate::fromString(ui->tableWidget->item(linha, 5)->text(),
                                                  "dd/MM/yyyy"));        //Vencimento
        QString pg = ui->tableWidget->item(linha, 6)->text();
        ui->dateEdit_3->setDate(pg.isEmpty() ? ui->dateEdit_3->minimumDate() :
                               QDate::fromString(pg, "dd/MM/yyyy"));     //Pagamento
        i = ui->comboBox_2->findText(ui->tableWidget->item(linha, 7)->text());
        if (i >= 0) ui->comboBox_2->setCurrentIndex(i);                  //Status
        i = ui->comboBox_3->findText(ui->tableWidget->item(linha, 8)->text());
        if (i >= 0) ui->comboBox_3->setCurrentIndex(i);                  //Forma
    });
}

ContasReceberWindow::~ContasReceberWindow()
{
    delete ui;
}

//nome do cliente digitado -> id (0 quando vazio ou nao encontrado)
int ContasReceberWindow::resolverCliente()
{
    QString nome = ui->lineEdit_2->text().trimmed();
    if (nome.isEmpty())
        return 0;
    QSqlQuery q;
    q.prepare("SELECT id_cliente FROM tb_clientes WHERE nome LIKE ? "
              "ORDER BY nome LIMIT 1");
    q.addBindValue("%" + nome + "%");
    if (q.exec() && q.next())
        return q.value(0).toInt();
    return 0;
}

//limpa os campos para uma conta nova
void ContasReceberWindow::limparCampos()
{
    ui->lineEdit->clear();
    ui->lineEdit_2->clear();
    ui->comboBox->setCurrentIndex(0);
    ui->doubleSpinBox->setValue(0);
    ui->dateEdit->setDate(QDate::currentDate());
    ui->dateEdit_2->setDate(QDate::currentDate().addDays(30));
    ui->dateEdit_3->setDate(ui->dateEdit_3->minimumDate());
    ui->comboBox_2->setCurrentIndex(0);
    ui->comboBox_3->setCurrentIndex(0);
    idAtual = 0;
    ui->tableWidget->clearSelection();
}

//preenche a lista de contas a receber
void ContasReceberWindow::carregarLista()
{
    QSqlQuery q;
    if (!q.exec("SELECT r.id_receber, COALESCE(c.nome,'-'), r.categoria, r.valor, "
                "r.data_emissao, r.data_vencimento, COALESCE(r.data_pagamento,''), "
                "r.status, COALESCE(r.forma_pagamento,'') "
                "FROM tb_receber r LEFT JOIN tb_clientes c "
                "ON c.id_cliente = r.id_cliente ORDER BY r.id_receber DESC")) {
        QMessageBox::warning(this, "Contas a Receber",
                             "Erro ao consultar:\n" + q.lastError().text());
        return;
    }

    const QStringList colunas = {"ID", "Cliente", "Categoria", "Valor", "Emissão",
                                 "Vencimento", "Pagamento", "Status", "Forma"};
    ui->tableWidget->clear();
    ui->tableWidget->setColumnCount(colunas.size());
    ui->tableWidget->setHorizontalHeaderLabels(colunas);

    ui->tableWidget->setRowCount(0);
    while (q.next()) {
        int linha = ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(linha);
        for (int c = 0; c < colunas.size(); ++c) {
            QString texto = q.value(c).toString();
            if (c == 3)   //valor formatado
                texto = QString::number(q.value(c).toDouble(), 'f', 2);
            QTableWidgetItem *item = new QTableWidgetItem(texto);
            item->setFlags(item->flags() & ~Qt::ItemIsEditable);
            ui->tableWidget->setItem(linha, c, item);
        }
        ui->tableWidget->item(linha, 0)->setData(Qt::UserRole, q.value(0).toInt());
    }
    ui->tableWidget->resizeColumnsToContents();
    limparCampos();
}

//atualiza a lista
void ContasReceberWindow::on_pushButton_clicked()
{
    carregarLista();
}

//insere uma conta a receber
void ContasReceberWindow::on_pushButton_2_clicked()
{
    if (ui->doubleSpinBox->value() <= 0) {
        QMessageBox::warning(this, "Contas a Receber", "Informe o valor da conta.");
        return;
    }

    QSqlQuery q;
    q.prepare("INSERT INTO tb_receber (id_cliente, categoria, valor, data_emissao, "
              "data_vencimento, data_pagamento, status, forma_pagamento) "
              "VALUES (?,?,?,?,?,?,?,?)");
    q.addBindValue(resolverCliente());
    q.addBindValue(ui->comboBox->currentText());
    q.addBindValue(ui->doubleSpinBox->value());
    q.addBindValue(ui->dateEdit->date().toString("dd/MM/yyyy"));
    q.addBindValue(ui->dateEdit_2->date().toString("dd/MM/yyyy"));
    q.addBindValue(ui->dateEdit_3->date() == ui->dateEdit_3->minimumDate()
                   ? QString() : ui->dateEdit_3->date().toString("dd/MM/yyyy"));
    q.addBindValue(ui->comboBox_2->currentText());
    q.addBindValue(ui->comboBox_3->currentText());

    if (!q.exec()) {
        QMessageBox::warning(this, "Contas a Receber", "Erro ao inserir:\n" +
                             q.lastError().text());
        return;
    }
    registrarLog("Inserção", "Contas a Receber",
                 "categoria: " + ui->comboBox->currentText() +
                 " valor: " + QString::number(ui->doubleSpinBox->value(), 'f', 2));
    carregarLista();
    QMessageBox::information(this, "Contas a Receber", "Conta inserida com sucesso.");
}

//altera a conta selecionada
void ContasReceberWindow::on_pushButton_3_clicked()
{
    if (idAtual == 0) {
        QMessageBox::warning(this, "Contas a Receber",
                             "Selecione uma conta na lista antes de alterar.");
        return;
    }
    if (ui->doubleSpinBox->value() <= 0) {
        QMessageBox::warning(this, "Contas a Receber", "Informe o valor da conta.");
        return;
    }

    QSqlQuery q;
    q.prepare("UPDATE tb_receber SET id_cliente=?, categoria=?, valor=?, data_emissao=?, "
              "data_vencimento=?, data_pagamento=?, status=?, forma_pagamento=? "
              "WHERE id_receber=?");
    q.addBindValue(resolverCliente());
    q.addBindValue(ui->comboBox->currentText());
    q.addBindValue(ui->doubleSpinBox->value());
    q.addBindValue(ui->dateEdit->date().toString("dd/MM/yyyy"));
    q.addBindValue(ui->dateEdit_2->date().toString("dd/MM/yyyy"));
    q.addBindValue(ui->dateEdit_3->date() == ui->dateEdit_3->minimumDate()
                   ? QString() : ui->dateEdit_3->date().toString("dd/MM/yyyy"));
    q.addBindValue(ui->comboBox_2->currentText());
    q.addBindValue(ui->comboBox_3->currentText());
    q.addBindValue(idAtual);

    if (!q.exec()) {
        QMessageBox::warning(this, "Contas a Receber", "Erro ao alterar:\n" +
                             q.lastError().text());
        return;
    }
    registrarLog("Alteração", "Contas a Receber", "id: " + QString::number(idAtual));
    carregarLista();
    QMessageBox::information(this, "Contas a Receber", "Conta alterada com sucesso.");
}

//exclui a conta selecionada
void ContasReceberWindow::on_pushButton_4_clicked()
{
    if (idAtual == 0) {
        QMessageBox::warning(this, "Contas a Receber",
                             "Selecione uma conta na lista antes de excluir.");
        return;
    }
    if (QMessageBox::question(this, "Contas a Receber", "Excluir esta conta?") !=
        QMessageBox::Yes)
        return;

    QSqlQuery q;
    q.prepare("DELETE FROM tb_receber WHERE id_receber=?");
    q.addBindValue(idAtual);
    if (!q.exec()) {
        QMessageBox::warning(this, "Contas a Receber", "Erro ao excluir:\n" +
                             q.lastError().text());
        return;
    }
    registrarLog("Exclusão", "Contas a Receber", "id: " + QString::number(idAtual));
    carregarLista();
    QMessageBox::information(this, "Contas a Receber", "Conta excluída com sucesso.");
}
