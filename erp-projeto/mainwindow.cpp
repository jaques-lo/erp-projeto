#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "telashelper.h"
#include "clientewindow.h"
#include "fornecedorwindow.h"
#include "funcionariowindow.h"
#include "produtowindow.h"
#include "categoriawindow.h"
#include "comprawindow.h"
#include "estoquewindow.h"
#include "movimentacaoestoquewindow.h"
#include "permissaowindow.h"
#include "configwindow.h"
#include "financeirowindow.h"
#include "vendawindow.h"
#include "caixawindow.h"
#include "logwindow.h"
#include "relatoriowindow.h"
#include "sobrewindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

//fecha o sistema
void MainWindow::on_actionSair_do_sistema_triggered()
{
    close();
}

//abre cada modulo do ERP (janela unica: se ja estiver aberta, so traz para frente)
void MainWindow::on_actionCadastro_de_clientes_triggered()
{
    abrirTela<ClienteWindow>();
}

void MainWindow::on_actionCadastro_de_fornecedores_triggered()
{
    abrirTela<FornecedorWindow>();
}

void MainWindow::on_actionCadastro_de_funcionarios_triggered()
{
    abrirTela<FuncionarioWindow>();
}

void MainWindow::on_actionCadastro_de_produtos_triggered()
{
    abrirTela<ProdutoWindow>();
}

void MainWindow::on_actionCadastro_de_categorias_triggered()
{
    abrirTela<CategoriaWindow>();
}

void MainWindow::on_actionControle_de_estoque_triggered()
{
    abrirTela<EstoqueWindow>();
}

void MainWindow::on_actionMovimentacao_de_estoque_triggered()
{
    abrirTela<MovimentacaoEstoqueWindow>();
}

void MainWindow::on_actionPermissao_triggered()
{
    abrirTela<PermissaoWindow>();
}

void MainWindow::on_actionConfiguracoes_triggered()
{
    abrirTela<ConfigWindow>();
}

void MainWindow::on_actionModulo_financeiro_triggered()
{
    abrirTela<FinanceiroWindow>();
}

void MainWindow::on_actionCaixa_triggered()
{
    abrirTela<CaixaWindow>();
}

void MainWindow::on_actionVendas_triggered()
{
    abrirTela<VendaWindow>();
}

void MainWindow::on_actionRelatorios_triggered()
{
    abrirTela<RelatorioWindow>();
}

void MainWindow::on_actionSobre_triggered()
{
    abrirTela<SobreWindow>();
}

void MainWindow::on_actionCompras_triggered()
{
    abrirTela<CompraWindow>();
}

void MainWindow::on_actionLogs_triggered()
{
    abrirTela<LogWindow>();
}
