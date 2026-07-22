#include "movimentacaoestoquewindow.h"
#include "ui_movimentacaoestoquewindow.h"

MovimentacaoEstoqueWindow::MovimentacaoEstoqueWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MovimentacaoEstoqueWindow)
{
    ui->setupUi(this);
}

MovimentacaoEstoqueWindow::~MovimentacaoEstoqueWindow()
{
    delete ui;
}
