#ifndef CATEGORIAWINDOW_H
#define CATEGORIAWINDOW_H

#include <QWidget>

namespace Ui {
class CategoriaWindow;
}

class CategoriaWindow : public QWidget
{
    Q_OBJECT

public:
    explicit CategoriaWindow(QWidget *parent = nullptr);
    ~CategoriaWindow();

private:
    Ui::CategoriaWindow *ui;
};

#endif // CATEGORIAWINDOW_H
