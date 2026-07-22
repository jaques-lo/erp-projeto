#ifndef FINANCEIROWINDOW_H
#define FINANCEIROWINDOW_H

#include <QWidget>

namespace Ui {
class FinanceiroWindow;
}

class FinanceiroWindow : public QWidget
{
    Q_OBJECT

public:
    explicit FinanceiroWindow(QWidget *parent = nullptr);
    ~FinanceiroWindow();

private:
    Ui::FinanceiroWindow *ui;
};

#endif // FINANCEIROWINDOW_H
