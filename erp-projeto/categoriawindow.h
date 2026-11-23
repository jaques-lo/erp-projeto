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

private slots:
    void on_pushButton_4_clicked();   //inserir
    void on_pushButton_2_clicked();   //alterar
    void on_pushButton_3_clicked();   //excluir
    void on_pushButton_clicked();     //pesquisar

private:
    void carregarLista(const QString &filtro = QString());
    Ui::CategoriaWindow *ui;
    int idAtual = 0;   //categoria selecionada na lista
};

#endif // CATEGORIAWINDOW_H
