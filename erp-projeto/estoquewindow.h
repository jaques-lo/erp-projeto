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

private:
    Ui::EstoqueWindow *ui;
};

#endif // ESTOQUEWINDOW_H
