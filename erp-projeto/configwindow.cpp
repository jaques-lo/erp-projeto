#include "configwindow.h"
#include "ui_configwindow.h"
#include "loghelper.h"
#include <QtSql>
#include <QShowEvent>
#include <QMessageBox>

//le uma preferencia salva na tb_config
static QString lerPreferencia(const QString &chave)
{
    QSqlQuery q;
    q.prepare("SELECT valor FROM tb_config WHERE chave=?");
    q.addBindValue(chave);
    if (q.exec() && q.next())
        return q.value(0).toString();
    return QString();
}

//grava uma preferencia na tb_config
static void gravarPreferencia(const QString &chave, const QString &valor)
{
    QSqlQuery q;
    q.prepare("INSERT INTO tb_config (chave, valor) VALUES (?,?) "
              "ON CONFLICT(chave) DO UPDATE SET valor=excluded.valor");
    q.addBindValue(chave);
    q.addBindValue(valor);
    q.exec();
}

ConfigWindow::ConfigWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ConfigWindow)
{
    ui->setupUi(this);
    montarImpressoras();
    carregar();
}

ConfigWindow::~ConfigWindow()
{
    delete ui;
}

//sempre que a tela for mostrada, recarrega o que esta salvo
void ConfigWindow::showEvent(QShowEvent *evento)
{
    QWidget::showEvent(evento);
    carregar();
}

//lista de impressoras disponiveis (a escolhida sempre aparece)
void ConfigWindow::montarImpressoras()
{
    QString salva = lerPreferencia("impressora");
    ui->comboBox->clear();
    const QStringList padroes = {"Padrão do sistema", "HP LaserJet Pro M404",
                                 "Epson LX-350", "Brother HL-2270DW",
                                 "Canon i-SENSYS LBP223"};
    for (const QString &i : padroes)
        ui->comboBox->addItem(i);
    if (!salva.isEmpty() && ui->comboBox->findText(salva) < 0)
        ui->comboBox->addItem(salva);
}

//carrega a empresa e as preferencias salvas
void ConfigWindow::carregar()
{
    //dados da empresa (primeira linha da tb_empresa)
    QSqlQuery q;
    if (q.exec("SELECT razao_social, cnpj, endereco, telefone FROM tb_empresa "
               "ORDER BY id_empresa LIMIT 1") && q.next()) {
        ui->lineEdit->setText(q.value(0).toString());   //Nome da empresa
        ui->lineEdit_2->setText(q.value(1).toString()); //CNPJ
        ui->lineEdit_3->setText(q.value(2).toString()); //Endereco
        ui->lineEdit_4->setText(q.value(3).toString()); //Telefone
    }

    //preferencias
    QString impressora = lerPreferencia("impressora");
    if (!impressora.isEmpty()) {
        int i = ui->comboBox->findText(impressora);
        ui->comboBox->setCurrentIndex(i >= 0 ? i : 0);
    }
    ui->checkBox->setChecked(lerPreferencia("backup_automatico") == "1");
    QString tema = lerPreferencia("tema");
    if (!tema.isEmpty())
        ui->comboBox_2->setCurrentText(tema);
}

//grava a empresa e as preferencias
void ConfigWindow::salvar()
{
    QString nome = ui->lineEdit->text().trimmed();

    //empresa: insere ou atualiza
    QSqlQuery existe;
    existe.exec("SELECT id_empresa FROM tb_empresa ORDER BY id_empresa LIMIT 1");
    if (existe.next()) {
        QSqlQuery up;
        up.prepare("UPDATE tb_empresa SET razao_social=?, nome_fantasia=?, cnpj=?, "
                   "endereco=?, telefone=? WHERE id_empresa=?");
        up.addBindValue(nome);
        up.addBindValue(nome);
        up.addBindValue(ui->lineEdit_2->text().trimmed());
        up.addBindValue(ui->lineEdit_3->text().trimmed());
        up.addBindValue(ui->lineEdit_4->text().trimmed());
        up.addBindValue(existe.value(0).toInt());
        up.exec();
    } else {
        QSqlQuery ins;
        ins.prepare("INSERT INTO tb_empresa (razao_social, nome_fantasia, cnpj, endereco, "
                    "telefone) VALUES (?,?,?,?,?)");
        ins.addBindValue(nome);
        ins.addBindValue(nome);
        ins.addBindValue(ui->lineEdit_2->text().trimmed());
        ins.addBindValue(ui->lineEdit_3->text().trimmed());
        ins.addBindValue(ui->lineEdit_4->text().trimmed());
        ins.exec();
    }

    //preferencias
    gravarPreferencia("impressora", ui->comboBox->currentText());
    gravarPreferencia("backup_automatico", ui->checkBox->isChecked() ? "1" : "0");
    gravarPreferencia("tema", ui->comboBox_2->currentText());

    registrarLog("Configurações salvas", "Configurações",
                 "empresa: " + nome + " tema: " + ui->comboBox_2->currentText());
    QMessageBox::information(this, "Configurações", "Configurações salvas com sucesso.");
}

//botao Salvar
void ConfigWindow::on_pushButton_clicked()
{
    salvar();
}

//volta tudo para o padrao
void ConfigWindow::on_pushButton_2_clicked()
{
    if (QMessageBox::question(this, "Configurações",
                              "Restaurar as configurações padrão?") != QMessageBox::Yes)
        return;

    ui->lineEdit->clear();
    ui->lineEdit_2->clear();
    ui->lineEdit_3->clear();
    ui->lineEdit_4->clear();
    ui->comboBox->setCurrentIndex(0);
    ui->checkBox->setChecked(false);
    ui->comboBox_2->setCurrentIndex(0);

    salvar();
}
