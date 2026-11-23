#include "categoriawindow.h"
#include "ui_categoriawindow.h"
#include "loghelper.h"
#include <QtSql>
#include <QSqlQueryModel>
#include <QMessageBox>

CategoriaWindow::CategoriaWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CategoriaWindow)
{
    ui->setupUi(this);

    //lista de categorias na tabela (e o clique na linha carrega nos campos)
    carregarLista();
}

CategoriaWindow::~CategoriaWindow()
{
    delete ui;
}

//atualiza a tabela (com filtro opcional)
void CategoriaWindow::carregarLista(const QString &filtro)
{
    QSqlQueryModel *model = new QSqlQueryModel(this);
    QString sql = "SELECT id_categoria, nome, descricao FROM tb_categorias";
    if (!filtro.isEmpty())
        sql += " WHERE nome LIKE '%" + filtro + "%'";
    sql += " ORDER BY nome";
    model->setQuery(sql);
    model->setHeaderData(0, Qt::Horizontal, "ID");
    model->setHeaderData(1, Qt::Horizontal, "Nome");
    model->setHeaderData(2, Qt::Horizontal, "Descrição");
    ui->tableView->setModel(model);
    ui->tableView->setColumnHidden(0, true);

    //setModel troca o selectionModel, entao (re)conecta o clique da linha aqui:
    //ao clicar numa linha, carrega a categoria nos campos
    connect(ui->tableView->selectionModel(), &QItemSelectionModel::currentChanged,
            this, [this](const QModelIndex &atual, const QModelIndex &) {
                if (!atual.isValid())
                    return;
                idAtual = ui->tableView->model()->data(
                              ui->tableView->model()->index(atual.row(), 0)).toInt();
                ui->lineEdit->setText(ui->tableView->model()->data(
                                          ui->tableView->model()->index(atual.row(), 1)).toString());
                ui->lineEdit_2->setText(ui->tableView->model()->data(
                                            ui->tableView->model()->index(atual.row(), 2)).toString());
            });

    idAtual = 0;
    ui->lineEdit->clear();
    ui->lineEdit_2->clear();
}

//pesquisa pelo nome
void CategoriaWindow::on_pushButton_clicked()
{
    QString nome = ui->lineEdit->text().trimmed();
    carregarLista(nome);
    if (ui->tableView->model()->rowCount() == 0)
        QMessageBox::information(this, "Categorias", "Nenhuma categoria encontrada.");
}

//insere uma nova categoria
void CategoriaWindow::on_pushButton_4_clicked()
{
    QString nome = ui->lineEdit->text().trimmed();
    if (nome.isEmpty()) {
        QMessageBox::warning(this, "Categorias", "Informe o nome da categoria.");
        return;
    }

    QSqlQuery q;
    q.prepare("INSERT INTO tb_categorias (nome, descricao) VALUES (?,?)");
    q.addBindValue(nome);
    q.addBindValue(ui->lineEdit_2->text().trimmed());
    if (!q.exec()) {
        QMessageBox::warning(this, "Categorias", "Erro ao inserir:\n" + q.lastError().text());
        return;
    }
    registrarLog("Inserção", "Categorias", "nome: " + nome);
    carregarLista();
    QMessageBox::information(this, "Categorias", "Categoria inserida com sucesso.");
}

//altera a categoria selecionada
void CategoriaWindow::on_pushButton_2_clicked()
{
    if (idAtual == 0) {
        QMessageBox::warning(this, "Categorias", "Selecione uma categoria na lista antes de alterar.");
        return;
    }
    QString nome = ui->lineEdit->text().trimmed();
    if (nome.isEmpty()) {
        QMessageBox::warning(this, "Categorias", "Informe o nome da categoria.");
        return;
    }

    QSqlQuery q;
    q.prepare("UPDATE tb_categorias SET nome=?, descricao=? WHERE id_categoria=?");
    q.addBindValue(nome);
    q.addBindValue(ui->lineEdit_2->text().trimmed());
    q.addBindValue(idAtual);
    if (!q.exec()) {
        QMessageBox::warning(this, "Categorias", "Erro ao alterar:\n" + q.lastError().text());
        return;
    }
    registrarLog("Alteração", "Categorias", "nome: " + nome);
    carregarLista();
    QMessageBox::information(this, "Categorias", "Categoria alterada com sucesso.");
}

//exclui a categoria selecionada
void CategoriaWindow::on_pushButton_3_clicked()
{
    if (idAtual == 0) {
        QMessageBox::warning(this, "Categorias", "Selecione uma categoria na lista antes de excluir.");
        return;
    }
    if (QMessageBox::question(this, "Categorias", "Excluir esta categoria?") !=
        QMessageBox::Yes)
        return;

    QSqlQuery q;
    q.prepare("DELETE FROM tb_categorias WHERE id_categoria=?");
    q.addBindValue(idAtual);
    if (!q.exec()) {
        QMessageBox::warning(this, "Categorias", "Erro ao excluir:\n" + q.lastError().text());
        return;
    }
    registrarLog("Exclusão", "Categorias", "id: " + QString::number(idAtual));
    carregarLista();
    QMessageBox::information(this, "Categorias", "Categoria excluída com sucesso.");
}
