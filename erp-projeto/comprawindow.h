#ifndef COMPRAWINDOW_H
#define COMPRAWINDOW_H

#include <QWidget>

namespace Ui {
class CompraWindow;
}

class CompraWindow : public QWidget
{
    Q_OBJECT

public:
    explicit CompraWindow(QWidget *parent = nullptr);
    ~CompraWindow();

private slots:
    void on_pushButton_5_clicked();   //buscar fornecedor
    void on_pushButton_clicked();     //adicionar item
    void on_pushButton_2_clicked();   //remover item
    void on_pushButton_3_clicked();   //finalizar compra
    void on_pushButton_4_clicked();   //cancelar compra

private:
    void limparTudo();
    double totalCarrinho() const;
    Ui::CompraWindow *ui;
    int idFornecedor = 0;   //fornecedor selecionado
};

#endif // COMPRAWINDOW_H
