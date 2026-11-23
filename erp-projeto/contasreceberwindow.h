#ifndef CONTASRECEBERWINDOW_H
#define CONTASRECEBERWINDOW_H

#include <QWidget>

namespace Ui {
class ContasReceberWindow;
}

class ContasReceberWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ContasReceberWindow(QWidget *parent = nullptr);
    ~ContasReceberWindow();

private slots:
    void on_pushButton_clicked();    //pesquisar (atualiza a lista)
    void on_pushButton_2_clicked();  //inserir
    void on_pushButton_3_clicked();  //alterar
    void on_pushButton_4_clicked();  //excluir

private:
    void carregarLista();
    void limparCampos();
    int resolverCliente();   //nome digitado -> id (0 se vazio/nao achou)
    Ui::ContasReceberWindow *ui;
    int idAtual = 0;         //conta selecionada na lista
};

#endif // CONTASRECEBERWINDOW_H
