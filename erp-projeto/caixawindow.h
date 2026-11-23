#ifndef CAIXAWINDOW_H
#define CAIXAWINDOW_H

#include <QWidget>

namespace Ui {
class CaixaWindow;
}

class CaixaWindow : public QWidget
{
    Q_OBJECT

public:
    explicit CaixaWindow(QWidget *parent = nullptr);
    ~CaixaWindow();

private slots:
    void on_pushButton_clicked();    //abrir caixa
    void on_pushButton_2_clicked();  //sangria
    void on_pushButton_3_clicked();  //fechar caixa
    void on_pushButton_4_clicked();  //suprimento

private:
    void carregarEstado();     //le o status do caixa salvo no banco
    void salvarEstado() const;
    void carregarResumo();     //vendas do dia por forma de pagamento
    double saldoEsperado() const;   //quanto deveria ter no caixa
    Ui::CaixaWindow *ui;
};

#endif // CAIXAWINDOW_H
