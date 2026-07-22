#ifndef CAIXAWINDOW_H
#define CAIXAWINDOW_H

#include <QWidget>

namespace Ui {
class CaixaWindow;
}

class CaixaWindow : public QWidget
{
    Q_OBJECT

public:
    explicit CaixaWindow(QWidget *parent = nullptr);
    ~CaixaWindow();

private:
    Ui::CaixaWindow *ui;
};

#endif // CAIXAWINDOW_H
