#ifndef PERMISSAOWINDOW_H
#define PERMISSAOWINDOW_H

#include <QWidget>

namespace Ui {
class PermissaoWindow;
}

class PermissaoWindow : public QWidget
{
    Q_OBJECT

public:
    explicit PermissaoWindow(QWidget *parent = nullptr);
    ~PermissaoWindow();

private:
    Ui::PermissaoWindow *ui;
};

#endif // PERMISSAOWINDOW_H
