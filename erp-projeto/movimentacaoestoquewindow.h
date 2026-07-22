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

private:
    Ui::MovimentacaoEstoqueWindow *ui;
};

#endif // MOVIMENTACAOESTOQUEWINDOW_H
