#ifndef PERMISSAOWINDOW_H
#define PERMISSAOWINDOW_H

#include <QWidget>

namespace Ui {
class PermissaoWindow;
}

class PermissaoWindow : public QWidget
{
    Q_OBJECT

public:
    explicit PermissaoWindow(QWidget *parent = nullptr);
    ~PermissaoWindow();

private slots:
    void on_pushButton_clicked();    //salvar
    void on_pushButton_2_clicked();  //cancelar
    void carregarPermissoes();       //ao terminar de digitar o funcionario

private:
    int resolverUsuario();   //nome digitado -> id do usuario (0 se nao achar)
    void limparTudo();
    Ui::PermissaoWindow *ui;
};

#endif // PERMISSAOWINDOW_H
