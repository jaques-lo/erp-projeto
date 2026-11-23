#ifndef MOVIMENTACAOESTOQUEWINDOW_H
#define MOVIMENTACAOESTOQUEWINDOW_H

#include <QWidget>

namespace Ui {
class MovimentacaoEstoqueWindow;
}

class MovimentacaoEstoqueWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MovimentacaoEstoqueWindow(QWidget *parent = nullptr);
    ~MovimentacaoEstoqueWindow();

private slots:
    void on_pushButton_clicked();    //registrar
    void on_pushButton_2_clicked();  //limpar

private:
    void carregarHistorico();
    Ui::MovimentacaoEstoqueWindow *ui;
};

#endif // MOVIMENTACAOGESTOQUEWINDOW_H
