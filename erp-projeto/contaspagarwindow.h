#ifndef CONTASPAGARWINDOW_H
#define CONTASPAGARWINDOW_H

#include <QWidget>

namespace Ui {
class ContasPagarWindow;
}

class ContasPagarWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ContasPagarWindow(QWidget *parent = nullptr);
    ~ContasPagarWindow();

private slots:
    void on_pushButton_clicked();    //pesquisar (atualiza a lista)
    void on_pushButton_2_clicked();  //inserir
    void on_pushButton_3_clicked();  //alterar
    void on_pushButton_4_clicked();  //excluir

private:
    void carregarLista();
    void limparCampos();
    int resolverFornecedor();   //nome digitado -> id (0 se vazio/nao achou)
    Ui::ContasPagarWindow *ui;
    int idAtual = 0;            //conta selecionada na lista
};

#endif // CONTASPAGARWINDOW_H
