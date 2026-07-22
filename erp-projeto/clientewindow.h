#ifndef CLIENTEWINDOW_H
#define CLIENTEWINDOW_H

#include <QWidget>

namespace Ui {
class ClienteWindow;
}

class ClienteWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ClienteWindow(QWidget *parent = nullptr);
    ~ClienteWindow();

private:
    Ui::ClienteWindow *ui;
};

#endif // CLIENTEWINDOW_H
