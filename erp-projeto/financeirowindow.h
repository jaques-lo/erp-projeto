#ifndef FINANCEIROWINDOW_H
#define FINANCEIROWINDOW_H

#include <QWidget>

namespace Ui {
class FinanceiroWindow;
}

class FinanceiroWindow : public QWidget
{
    Q_OBJECT

public:
    explicit FinanceiroWindow(QWidget *parent = nullptr);
    ~FinanceiroWindow();

protected:
    void showEvent(QShowEvent *evento) override;   //atualiza ao abrir a tela

private slots:
    //botoes de navegacao do modulo financeiro
    void on_pushButton_clicked();      //contas a pagar
    void on_pushButton_2_clicked();    //contas a receber
    void on_pushButton_3_clicked();    //relatorio financeiro

private:
    void atualizarPainel();
    Ui::FinanceiroWindow *ui;
};

#endif // FINANCEIROWINDOW_H
