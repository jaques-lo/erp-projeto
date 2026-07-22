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

private:
    Ui::CompraWindow *ui;
};

#endif // COMPRAWINDOW_H
