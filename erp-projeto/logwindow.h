#ifndef LOGWINDOW_H
#define LOGWINDOW_H

#include <QWidget>

namespace Ui {
class LogWindow;
}

class LogWindow : public QWidget
{
    Q_OBJECT

public:
    explicit LogWindow(QWidget *parent = nullptr);
    ~LogWindow();

private slots:
    void on_pushButton_clicked();    //pesquisar (filtra)
    void on_pushButton_2_clicked();  //exportar para CSV

private:
    void carregar(const QString &usuario, const QString &acao, const QDate &data);
    Ui::LogWindow *ui;
};

#endif // LOGWINDOW_H
