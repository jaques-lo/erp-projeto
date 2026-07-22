#ifndef FUNCIONARIOWINDOW_H
#define FUNCIONARIOWINDOW_H

#include <QWidget>

namespace Ui {
class FuncionarioWindow;
}

class FuncionarioWindow : public QWidget
{
    Q_OBJECT

public:
    explicit FuncionarioWindow(QWidget *parent = nullptr);
    ~FuncionarioWindow();

private:
    Ui::FuncionarioWindow *ui;
};

#endif // FUNCIONARIOWINDOW_H
