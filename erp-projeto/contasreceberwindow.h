#ifndef CONTASRECEBERWINDOW_H
#define CONTASRECEBERWINDOW_H

#include <QWidget>

namespace Ui {
class ContasReceberWindow;
}

class ContasReceberWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ContasReceberWindow(QWidget *parent = nullptr);
    ~ContasReceberWindow();

private:
    Ui::ContasReceberWindow *ui;
};

#endif // CONTASRECEBERWINDOW_H
