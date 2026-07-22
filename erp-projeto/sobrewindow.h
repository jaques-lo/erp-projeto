#ifndef SOBREWINDOW_H
#define SOBREWINDOW_H

#include <QWidget>

namespace Ui {
class SobreWindow;
}

class SobreWindow : public QWidget
{
    Q_OBJECT

public:
    explicit SobreWindow(QWidget *parent = nullptr);
    ~SobreWindow();

private:
    Ui::SobreWindow *ui;
};

#endif // SOBREWINDOW_H
