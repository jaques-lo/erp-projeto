#ifndef FORNECEDORWINDOW_H
#define FORNECEDORWINDOW_H

#include <QWidget>

namespace Ui {
class FornecedorWindow;
}

class FornecedorWindow : public QWidget
{
    Q_OBJECT

public:
    explicit FornecedorWindow(QWidget *parent = nullptr);
    ~FornecedorWindow();

private slots:
    void on_pushButton_7_clicked();   //inserir
    void on_pushButton_8_clicked();   //alterar
    void on_pushButton_9_clicked();   //excluir
    void on_pushButton_6_clicked();   //pesquisar

private:
    void limparCampos();
    Ui::FornecedorWindow *ui;
    int idAtual = 0;   //fornecedor carregado na tela
};

#endif // FORNECEDORWINDOW_H
