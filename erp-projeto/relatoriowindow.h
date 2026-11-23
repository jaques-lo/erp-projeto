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

private slots:
    void on_btn_gerar_clicked();     //gera o relatorio escolhido
    void on_btn_exportar_clicked();  //exporta para CSV

private:
    void montar(const QString &titulo, const QStringList &colunas, const QString &sql);
    Ui::RelatorioWindow *ui;
};

#endif // RELATORIOWINDOW_H
