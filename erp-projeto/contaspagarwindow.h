#ifndef CONTASPAGARWINDOW_H
#define CONTASPAGARWINDOW_H

#include <QWidget>

namespace Ui {
class ContasPagarWindow;
}

class ContasPagarWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ContasPagarWindow(QWidget *parent = nullptr);
    ~ContasPagarWindow();

private:
    Ui::ContasPagarWindow *ui;
};

#endif // CONTASPAGARWINDOW_H
