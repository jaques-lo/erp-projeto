#ifndef FORNECEDORWINDOW_H
#define FORNECEDORWINDOW_H

#include <QWidget>

namespace Ui {
class FornecedorWindow;
}

class FornecedorWindow : public QWidget
{
    Q_OBJECT

public:
    explicit FornecedorWindow(QWidget *parent = nullptr);
    ~FornecedorWindow();

private:
    Ui::FornecedorWindow *ui;
};

#endif // FORNECEDORWINDOW_H
