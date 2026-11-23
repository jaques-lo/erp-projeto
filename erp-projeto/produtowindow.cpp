#include "produtowindow.h"
#include "ui_produtowindow.h"
#include "loghelper.h"
#include <QtSql>
#include <QMessageBox>

ProdutoWindow::ProdutoWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ProdutoWindow)
{
    ui->setupUi(this);

    //precos: o formulario tem os valores em 2 lugares (campos de texto e spinners),
    //entao sincroniza o texto no spinner e grava sempre pelo spinner
    ui->doubleSpinBox->setMaximum(99999999.99);    //custo
    ui->doubleSpinBox_2->setMaximum(99999999.99);  //venda
    connect(ui->lineEdit_6, &QLineEdit::editingFinished, this, [this]() {
        bool ok = false;
        double v = ui->lineEdit_6->text().replace(',', '.').toDouble(&ok);
        if (ok)
            ui->doubleSpinBox->setValue(v);
    });
    connect(ui->lineEdit_7, &QLineEdit::editingFinished, this, [this]() {
        bool ok = false;
        double v = ui->lineEdit_7->text().replace(',', '.').toDouble(&ok);
        if (ok)
            ui->doubleSpinBox_2->setValue(v);
    });

    //unidade pode ser digitada livre
    ui->comboBox_2->setEditable(true);

    ui->checkBox->setChecked(true);   //produto novo ja nasce ativo
    ui->label_9->setText("-");   //ID do produto
    carregarCategorias();
}

ProdutoWindow::~ProdutoWindow()
{
    delete ui;
}

//preenche o combo de categorias
void ProdutoWindow::carregarCategorias()
{
    ui->comboBox->clear();
    QSqlQuery q("SELECT id_categoria, nome FROM tb_categorias ORDER BY nome");
    while (q.next())
        ui->comboBox->addItem(q.value(1).toString(), q.value(0).toInt());
}

//limpa a tela para um produto novo
void ProdutoWindow::limparCampos()
{
    ui->lineEdit_2->clear();   //descricao
    ui->lineEdit_3->clear();   //codigo de barras
    ui->lineEdit_6->clear();   //custo (texto)
    ui->lineEdit_7->clear();   //venda (texto)
    ui->doubleSpinBox->setValue(0);
    ui->doubleSpinBox_2->setValue(0);
    ui->comboBox->setCurrentIndex(-1);
    ui->comboBox_2->setCurrentIndex(-1);
    ui->checkBox->setChecked(true);
    ui->label_9->setText("-");
    idAtual = 0;
}

//pesquisa pela descricao (ou codigo de barras) e carrega o produto
void ProdutoWindow::on_pushButton_4_clicked()
{
    QString texto = ui->lineEdit_2->text().trimmed();
    if (texto.isEmpty()) {
        QMessageBox::warning(this, "Produtos", "Digite a descrição para pesquisar.");
        return;
    }

    QSqlQuery q;
    q.prepare("SELECT * FROM tb_produtos WHERE descricao LIKE ? OR codigo_barras = ? "
              "ORDER BY descricao LIMIT 1");
    q.addBindValue("%" + texto + "%");
    q.addBindValue(texto);
    if (!q.exec() || !q.next()) {
        QMessageBox::information(this, "Produtos", "Produto não encontrado.");
        limparCampos();
        return;
    }

    idAtual = q.value("id_produtos").toInt();
    ui->label_9->setText(QString::number(idAtual));                     //ID
    ui->lineEdit_2->setText(q.value("descricao").toString());           //Descricao
    ui->lineEdit_3->setText(q.value("codigo_barras").toString());       //Codigo de barras
    double custo = q.value("custo").toDouble();
    double preco = q.value("preco").toDouble();
    ui->doubleSpinBox->setValue(custo);                                 //Custo
    ui->doubleSpinBox_2->setValue(preco);                               //Venda
    ui->lineEdit_6->setText(QString::number(custo, 'f', 2));            //Custo (texto)
    ui->lineEdit_7->setText(QString::number(preco, 'f', 2));            //Venda (texto)
    int i = ui->comboBox->findData(q.value("id_categoria").toInt());   //Categoria
    ui->comboBox->setCurrentIndex(i);
    ui->comboBox_2->setCurrentText(q.value("unidade").toString());      //Unidade
    ui->checkBox->setChecked(q.value("ativo").toInt() == 1);            //Status
}

//insere um novo produto (e abre a linha de estoque dele)
void ProdutoWindow::on_pushButton_clicked()
{
    QString descricao = ui->lineEdit_2->text().trimmed();
    if (descricao.isEmpty()) {
        QMessageBox::warning(this, "Produtos", "Informe a descrição do produto.");
        return;
    }
    int idCategoria = ui->comboBox->currentData().toInt();
    if (idCategoria <= 0) {
        QMessageBox::warning(this, "Produtos", "Selecione a categoria.\n"
                                               "(Cadastre uma em Cadastro > Categorias)");
        return;
    }

    QSqlQuery q;
    q.prepare("INSERT INTO tb_produtos (codigo, descricao, id_categoria, marca, modelo, "
              "custo, preco, estoque_minimo, unidade, codigo_barras, ativo) "
              "VALUES (' ', ?, ?, ' ', ' ', ?, ?, 0, ?, ?, ?)");
    q.addBindValue(descricao);
    q.addBindValue(idCategoria);
    q.addBindValue(ui->doubleSpinBox->value());                 //custo
    q.addBindValue(ui->doubleSpinBox_2->value());               //preco
    q.addBindValue(ui->comboBox_2->currentText().trimmed());    //unidade
    q.addBindValue(ui->lineEdit_3->text().trimmed());           //codigo de barras
    q.addBindValue(ui->checkBox->isChecked() ? 1 : 0);          //ativo

    if (!q.exec()) {
        QMessageBox::warning(this, "Produtos", "Erro ao inserir:\n" + q.lastError().text());
        return;
    }
    idAtual = q.lastInsertId().toInt();

    //cria a linha de estoque do produto novo
    QSqlQuery e;
    e.prepare("INSERT INTO tb_estoque (id_produto, quantidade, localizacao) VALUES (?,0,'')");
    e.addBindValue(idAtual);
    e.exec();

    ui->label_9->setText(QString::number(idAtual));
    registrarLog("Inserção", "Produtos", "descricao: " + descricao);
    QMessageBox::information(this, "Produtos", "Produto inserido com sucesso.");
}

//altera o produto carregado na tela
void ProdutoWindow::on_pushButton_2_clicked()
{
    if (idAtual == 0) {
        QMessageBox::warning(this, "Produtos", "Pesquise um produto antes de alterar.");
        return;
    }
    QString descricao = ui->lineEdit_2->text().trimmed();
    if (descricao.isEmpty()) {
        QMessageBox::warning(this, "Produtos", "Informe a descrição do produto.");
        return;
    }
    int idCategoria = ui->comboBox->currentData().toInt();
    if (idCategoria <= 0) {
        QMessageBox::warning(this, "Produtos", "Selecione a categoria.");
        return;
    }

    QSqlQuery q;
    q.prepare("UPDATE tb_produtos SET descricao=?, id_categoria=?, custo=?, preco=?, "
              "unidade=?, codigo_barras=?, ativo=? WHERE id_produtos=?");
    q.addBindValue(descricao);
    q.addBindValue(idCategoria);
    q.addBindValue(ui->doubleSpinBox->value());                 //custo
    q.addBindValue(ui->doubleSpinBox_2->value());               //preco
    q.addBindValue(ui->comboBox_2->currentText().trimmed());    //unidade
    q.addBindValue(ui->lineEdit_3->text().trimmed());           //codigo de barras
    q.addBindValue(ui->checkBox->isChecked() ? 1 : 0);          //ativo
    q.addBindValue(idAtual);

    if (!q.exec()) {
        QMessageBox::warning(this, "Produtos", "Erro ao alterar:\n" + q.lastError().text());
        return;
    }
    registrarLog("Alteração", "Produtos", "descricao: " + descricao);
    QMessageBox::information(this, "Produtos", "Produto alterado com sucesso.");
}

//exclui o produto carregado na tela (e o seu estoque)
void ProdutoWindow::on_pushButton_3_clicked()
{
    if (idAtual == 0) {
        QMessageBox::warning(this, "Produtos", "Pesquise um produto antes de excluir.");
        return;
    }
    if (QMessageBox::question(this, "Produtos", "Excluir este produto?") !=
        QMessageBox::Yes)
        return;

    QSqlQuery q;
    q.prepare("DELETE FROM tb_produtos WHERE id_produtos=?");
    q.addBindValue(idAtual);
    if (!q.exec()) {
        QMessageBox::warning(this, "Produtos", "Erro ao excluir:\n" + q.lastError().text());
        return;
    }
    QSqlQuery e;
    e.prepare("DELETE FROM tb_estoque WHERE id_produto=?");
    e.addBindValue(idAtual);
    e.exec();

    registrarLog("Exclusão", "Produtos", "id: " + QString::number(idAtual));
    limparCampos();
    QMessageBox::information(this, "Produtos", "Produto excluído com sucesso.");
}
