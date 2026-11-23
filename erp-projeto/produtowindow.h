#ifndef PRODUTOWINDOW_H
#define PRODUTOWINDOW_H

#include <QWidget>

namespace Ui {
class ProdutoWindow;
}

class ProdutoWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ProdutoWindow(QWidget *parent = nullptr);
    ~ProdutoWindow();

private slots:
    void on_pushButton_clicked();    //inserir
    void on_pushButton_2_clicked();  //alterar
    void on_pushButton_3_clicked();  //excluir
    void on_pushButton_4_clicked();  //pesquisar

private:
    void carregarCategorias();
    void limparCampos();
    Ui::ProdutoWindow *ui;
    int idAtual = 0;   //produto carregado na tela
};

#endif // PRODUTOWINDOW_H
