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

private:
    Ui::VendaWindow *ui;
};

#endif // VENDAWINDOW_H
