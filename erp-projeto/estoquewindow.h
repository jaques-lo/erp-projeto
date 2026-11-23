#ifndef ESTOQUEWINDOW_H
#define ESTOQUEWINDOW_H

#include <QWidget>

namespace Ui {
class EstoqueWindow;
}

class EstoqueWindow : public QWidget
{
    Q_OBJECT

public:
    explicit EstoqueWindow(QWidget *parent = nullptr);
    ~EstoqueWindow();

private slots:
    void on_pushButton_clicked();    //pesquisar
    void on_pushButton_2_clicked();  //ajustar estoque
    //botao "Ver movimentacoes"
    void on_pushButton_3_clicked();

private:
    void carregarTabela(const QString &filtroProduto = QString(),
                        const QString &filtroCategoria = QString());
    Ui::EstoqueWindow *ui;
};

#endif // ESTOQUEWINDOW_H
