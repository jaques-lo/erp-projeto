#ifndef VENDAWINDOW_H
#define VENDAWINDOW_H

#include <QWidget>

namespace Ui {
class VendaWindow;
}

class VendaWindow : public QWidget
{
    Q_OBJECT

public:
    explicit VendaWindow(QWidget *parent = nullptr);
    ~VendaWindow();

private slots:
    void on_pushButton_clicked();     //adicionar item
    void on_pushButton_2_clicked();   //remover item
    void on_pushButton_3_clicked();   //finalizar venda
    void on_pushButton_4_clicked();   //cancelar venda

private:
    void limparTudo();
    double totalCarrinho() const;
    Ui::VendaWindow *ui;
};

#endif // VENDAWINDOW_H
