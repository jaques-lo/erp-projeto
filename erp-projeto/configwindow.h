#ifndef CONFIGWINDOW_H
#define CONFIGWINDOW_H

#include <QWidget>

namespace Ui {
class ConfigWindow;
}

class ConfigWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ConfigWindow(QWidget *parent = nullptr);
    ~ConfigWindow();

protected:
    void showEvent(QShowEvent *evento) override;   //recarrega ao abrir

private slots:
    void on_pushButton_clicked();    //salvar
    void on_pushButton_2_clicked();  //restaurar padrao

private:
    void carregar();
    void salvar();
    void montarImpressoras();
    Ui::ConfigWindow *ui;
};

#endif // CONFIGWINDOW_H
