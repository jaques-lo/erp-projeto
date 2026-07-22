#ifndef RELATORIOWINDOW_H
#define RELATORIOWINDOW_H

#include <QWidget>

namespace Ui {
class RelatorioWindow;
}

class RelatorioWindow : public QWidget
{
    Q_OBJECT

public:
    explicit RelatorioWindow(QWidget *parent = nullptr);
    ~RelatorioWindow();

private:
    Ui::RelatorioWindow *ui;
};

#endif // RELATORIOWINDOW_H
