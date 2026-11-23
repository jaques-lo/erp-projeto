#ifndef FUNCIONARIOWINDOW_H
#define FUNCIONARIOWINDOW_H

#include <QWidget>

namespace Ui {
class FuncionarioWindow;
}

class FuncionarioWindow : public QWidget
{
    Q_OBJECT

public:
    explicit FuncionarioWindow(QWidget *parent = nullptr);
    ~FuncionarioWindow();

private slots:
    void on_btn_pesquisar_clicked();   //pesquisar
    void on_btn_inserir_clicked();     //inserir
    void on_btn_alterar_clicked();     //alterar
    void on_btn_excluir_clicked();     //excluir

private:
    void limparCampos();
    Ui::FuncionarioWindow *ui;
    int idAtual = 0;   //funcionario carregado na tela
};

#endif // FUNCIONARIOWINDOW_H
