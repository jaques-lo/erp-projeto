#ifndef CLIENTEWINDOW_H
#define CLIENTEWINDOW_H

#include <QWidget>

namespace Ui {
class ClienteWindow;
}

class ClienteWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ClienteWindow(QWidget *parent = nullptr);
    ~ClienteWindow();

private slots:
    void on_pushButton_clicked();    //inserir
    void on_pushButton_2_clicked();  //alterar
    void on_pushButton_3_clicked();  //excluir
    void on_pushButton_4_clicked();  //pesquisar

private:
    void limparCampos();
    Ui::ClienteWindow *ui;
    int idAtual = 0;   //cliente carregado na tela
};

#endif // CLIENTEWINDOW_H
