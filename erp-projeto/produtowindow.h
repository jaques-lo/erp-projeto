#ifndef PRODUTOWINDOW_H
#define PRODUTOWINDOW_H

#include <QWidget>

namespace Ui {
class ProdutoWindow;
}

class ProdutoWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ProdutoWindow(QWidget *parent = nullptr);
    ~ProdutoWindow();

private:
    Ui::ProdutoWindow *ui;
};

#endif // PRODUTOWINDOW_H
