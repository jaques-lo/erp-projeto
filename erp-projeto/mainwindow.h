#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    //sair do sistema
    void on_actionSair_do_sistema_triggered();
    //menus de cadastro
    void on_actionCadastro_de_clientes_triggered();
    void on_actionCadastro_de_fornecedores_triggered();
    void on_actionCadastro_de_funcionarios_triggered();
    void on_actionCadastro_de_produtos_triggered();
    void on_actionCadastro_de_categorias_triggered();
    //estoque
    void on_actionControle_de_estoque_triggered();
    void on_actionMovimentacao_de_estoque_triggered();
    //configuracoes
    void on_actionPermissao_triggered();
    void on_actionConfiguracoes_triggered();
    //financeiro
    void on_actionModulo_financeiro_triggered();
    //toolbar
    void on_actionCaixa_triggered();
    void on_actionVendas_triggered();
    //relatorios e sobre
    void on_actionRelatorios_triggered();
    void on_actionSobre_triggered();
    //compras e logs
    void on_actionCompras_triggered();
    void on_actionLogs_triggered();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
